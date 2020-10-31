; БПИ199
; Волохов Никита Алексеевич
; Вариант 5
; "Разработать программу, определяющую наименьшее число с плавающей точкой
; методом дихотомии и сравнения с единицей"
format PE console
entry start

include 'win32a.inc'

section '.data' data readable writable

        strNumOfFPNs  db 'Num of FPNs [1; 100]? ', 0
        strMinFloat   db 'Min float is: %lf', 10, 0
        strIncorNum   db 'Incorrect num of FPNs: %d', 10, 0
        strScanInt    db '%d', 0
        strScanFloat  db '%lf', 0

        num_of_fpns   dd 0
        temp_fval     dq ?
        min_fval      dq ?
        i             dd ?

section '.code' code readable executable

start:
; 1) num of FPNs input
        push strNumOfFPNs
        call [printf]

        push num_of_fpns
        push strScanInt
        call [scanf]

        ; check if num entered is correct
        cmp [num_of_fpns], 1
        jl incorrNumOfFPNs

        cmp [num_of_fpns], 100
        jg incorrNumOfFPNs

        jmp firstFPN

incorrNumOfFPNs:
        push [num_of_fpns]
        push strIncorNum
        call [printf]
        jmp finish

; 2) save first FPN to st0 to compare it with other FPNs
firstFPN:
        FINIT ; coprocessor init

        invoke scanf, strScanFloat, temp_fval

        fld [temp_fval] ; push temp_fval to stack (st0)

; 3) input other (num_of_fpns - 1) FPNs and compare it with st1 FPN
preFPNLoop:
        cmp [num_of_fpns], 1
        je endFPNLoop ; we already have 1 num. It is the answer

        sub [num_of_fpns], 1
        xor ecx, ecx ; ecx = 0

FPNLoop:
        mov [i], ecx

        cmp ecx, [num_of_fpns]
        je endFPNLoop ; to end loop

        invoke scanf, strScanFloat, temp_fval

        fcom [temp_fval] ; compare with FPN in st0
        fstsw ax
        sahf
        ja newStVal

        inc [i]
        mov ecx, [i]
        jmp FPNLoop

newStVal:
        fld [temp_fval] ; push temp_fval to stack (st0)

        inc [i]
        mov ecx, [i]
        jmp FPNLoop

endFPNLoop:
        fst [min_fval] ; move st0 FPN to min_fval var

        invoke printf, strMinFloat, dword[min_fval], dword[min_fval + 4]

; 4) programm end
finish:
        call [getch]

        push 0
        call [ExitProcess]


section '.idata' import data readable
library kernel, 'kernel32.dll',\
        msvcrt, 'msvcrt.dll'

include 'api\kernel32.inc'
    import kernel,\
           ExitProcess, 'ExitProcess'

include 'api\kernel32.inc'
    import msvcrt,\
           printf, 'printf',\
           scanf, 'scanf',\
           getch, '_getch'