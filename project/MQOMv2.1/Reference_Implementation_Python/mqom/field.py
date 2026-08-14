from numba import jit, njit
from .field_lut import LUT_F16, LUT_F256, LUT_LIFTING_F16_F256

class BinaryField:
    @property
    def repr(self):
        raise NotImplementedError()

    def __add__(self, other):
        raise NotImplementedError()

    def __radd__(self, other):
        return self.__add__(other)

    def __sub__(self, other):
        return self.__add__(other)

    def __mul__(self, other):
        raise NotImplementedError()

    def __rmul__(self, other):
        return self.__mul__(other)

    def __neg__(self):
        return self

    def __eq__(self, other):
        return self.repr == other.repr

    def __pow__(self, power):
        if power == 2:
            return self*self
        raise NotImplementedError()
    
    bitsize = None
    def to_bitstring(self):
        return bin(self.repr)[2:].rjust(self.bitsize, '0')[::-1]
    
    @classmethod
    def from_bitstring(cls, bitstring):
        return cls(int(bitstring[::-1], base=2))

    def __repr__(self):
        return hex(self.repr)
    
    @classmethod
    def order(cls):
        return 2**cls.bitsize

class F2(BinaryField):
    bitsize = 1

    def __init__(self, bit):
        if type(bit) is bool:
            self._repr = int(bit)
        elif type(bit) is int:
            self._repr = bit
        elif type(bit) is str:
            self._repr = int(bit)
        else:
            raise TypeError()

    @property
    def repr(self):
        return self._repr

    def __add__(self, other):
        if isinstance(other, F2):
            return type(self)(self.repr ^ other.repr)
        if (type(other) is int) and other == 0:
            return self
        return NotImplemented

    def __mul__(self, other):
        if not isinstance(other, F2):
            return NotImplemented
        return type(self)(self.repr & other.repr)


class F2to4(BinaryField):
    bitsize = 4

    def __init__(self, value):
        if type(value) is int:
            self._repr = value
        elif type(value) is str:
            self._repr = int(value)
        elif isinstance(value, F2):
            self._repr = value.repr
        else:
            raise TypeError()

    @property
    def repr(self):
        return self._repr

    def __add__(self, other):
        if isinstance(other, F2) or isinstance(other, F2to4):
            return type(self)(self.repr ^ other.repr)
        if (type(other) is int) and other == 0:
            return self
        raise NotImplemented

    def __mul__(self, other):
        if isinstance(other, F2to4):
            return type(self)(LUT_F16[self.repr][other.repr])
        if isinstance(other, F2):
            if other.repr:
                return self
            return type(self)(0)
        return NotImplemented


class F2to8(BinaryField):
    bitsize = 8

    def __init__(self, value):
        if type(value) is int:
            self._repr = value
        elif type(value) is str:
            self._repr = int(value)
        elif isinstance(value, F2):
            self._repr = value.repr
        elif isinstance(value, F2to4):
            self._repr = LUT_LIFTING_F16_F256[value.repr]
        else:
            raise TypeError()

    @property
    def repr(self):
        return self._repr

    def __add__(self, other):
        if isinstance(other, F2) or isinstance(other, F2to8):
            return type(self)(self.repr ^ other.repr)
        if isinstance(other, F2to4):
            return type(self)(self.repr ^ LUT_LIFTING_F16_F256[other.repr])
        if (type(other) is int) and other == 0:
            return self
        raise NotImplemented

    def __mul__(self, other):
        if isinstance(other, F2to8):
            return type(self)(LUT_F256[self.repr][other.repr])
        if isinstance(other, F2to4):
            return type(self)(LUT_F256[self.repr][LUT_LIFTING_F16_F256[other.repr]])
        if isinstance(other, F2):
            if other.repr:
                return self
            return type(self)(0)
        return NotImplemented
    

class F2to16(BinaryField):
    bitsize = 16

    def __init__(self, value):
        if type(value) is int:
            self._repr = value
            self._byte_repr = (value % 256, value // 256)
        elif type(value) in [tuple, list]:
            assert len(value) == 2
            self._byte_repr = value
            self._repr = value[0] + value[1]*256
        elif isinstance(value, F2) or isinstance(value, F2to8):
            self._repr = value.repr
            self._byte_repr = (value.repr, 0)
        elif isinstance(value, F2to4):
            repr = LUT_LIFTING_F16_F256[value.repr]
            self._repr = repr
            self._byte_repr = (repr, 0)
        else:
            raise TypeError()
        
    @property
    def byte_repr(self):
        return self._byte_repr

    @property
    def repr(self):
        return self._repr

    def __add__(self, other):
        if isinstance(other, F2) or isinstance(other, F2to8) or isinstance(other, F2to16):
            return type(self)(self.repr ^ other.repr)
        if isinstance(other, F2to4):
            return type(self)(self.repr ^ LUT_LIFTING_F16_F256[other.repr])
        if (type(other) is int) and other == 0:
            return self
        raise NotImplemented

    def __mul__(self, other):
        if isinstance(other, F2to16):
            (a0, a1) = self.byte_repr
            (b0, b1) = other.byte_repr
            lead = LUT_F256[a1][b1]
            cnst = LUT_F256[a0][b0]
            r0 = cnst ^ LUT_F256[32][lead]
            r1 = cnst ^ LUT_F256[a0^a1][b0^b1]
            return type(self)((r0, r1))
        if isinstance(other, F2to8):
            r0 = LUT_F256[self.byte_repr[0]][other.repr]
            r1 = LUT_F256[self.byte_repr[1]][other.repr]
            return type(self)((r0, r1))
        if isinstance(other, F2to4):
            other_repr = LUT_LIFTING_F16_F256[other.repr]
            r0 = LUT_F256[self.byte_repr[0]][other_repr]
            r1 = LUT_F256[self.byte_repr[1]][other_repr]
            return type(self)((r0, r1))
        if isinstance(other, F2):
            if other.repr:
                return self
            return type(self)(0)
        return NotImplemented
    
