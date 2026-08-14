from .utils import Array, Arrays, MultiDimArray
from .bits import map_to_bits
from .parsing import parse, ArrayFrmt, VectorFrmt
from numpy import matmul

class MQEquations:
    def __init__(self, params):
        self._params = params

        par = params
        self._packed_eqn_format = ArrayFrmt(
            *[
                VectorFrmt(par.extension_field, j+1)
                for j in range(par.n)
            ] + [
                VectorFrmt(par.extension_field, par.n)
            ]
        )

    @property
    def params(self):
        return self._params
    
    def expand_equations(self, mseed_eq):    
        par = self.params
        seed_eq, A, b = Arrays(3, par.m_hat)
        for i in range(par.m_hat):
            # Expand equation's seed
            seed_eq[i] = par.xof((b'\x01', mseed_eq, map_to_bits(i, 2)), len=par.lda)
            stream = par.prg(b'\x00'*par.lda, 0, seed_eq[i], self._packed_eqn_format.get_bytesize())
            (data,) = parse(stream, self._packed_eqn_format)

            A[i] = MultiDimArray((par.n, par.n), par.extension_field(0))
            for j in range(par.n):
                row = data[j]
                A[i][j][:len(row)] = row
            b[i] = data[-1]

        return (A, b)

    def compute_y(self, x, A, b):
        par = self.params
        y = Array(par.m_hat)
        for i in range(par.m_hat):
            y[i] = matmul(x, matmul(A[i], x)) + matmul(b[i], x)
        return y
