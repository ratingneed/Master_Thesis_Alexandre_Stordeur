MQOM2
=====

This submission package is composed of the following folders:

KAT/
    mqom2_<category>_<field>_<tradeoff>_<variant>/
        KAT vectors for mqom2_<category>_<field>_<tradeoff>_<variant>.

Optimized_Implementation/
    mqom2_<category>_<field>_<tradeoff>_<variant>/
        Optimized implementation for mqom2_<category>_<field>_<tradeoff>_<variant>.
        The applied optimizations are determined at compilation time, depending on the target platform.
        Ultimately, pure portable C code is selected when no specific platform optimization can be applied.

Reference_Implementation/
    mqom2_<category>_<field>_<tradeoff>_<variant>/
        Reference implementation for mqom2_<category>_<field>_<tradeoff>_<variant>.
        The code is identical to that of Optimized_Implementation. Readers interested in a reference
        implementation to better understand the scheme should instead refer to the Python reference code.

Reference_Implementation_Python/
    Generic Reference implementation in Python3.

Supporting_Documentation/
    cover_sheet.pdf
    specifications.pdf
    IP_Statements/
        IP statements of submitters

Notes
======

Each implementation under Reference_Implementation and
Optimized_Implementation has its own Makefile; when used, it compiles
the code along with the test vector generator. Execute
    > make sign; ./sign
which produces the .req and .rsp files, which should match the ones
provided in KAT/.
