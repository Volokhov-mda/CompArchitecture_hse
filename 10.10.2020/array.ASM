format PE console
entry start

include 'win32a.inc'

section '.data' data readable writable

        strVecSize    db 'Size of vector? ', 0
        strNumBanned  db 'Number banned? ', 0
        strIncorSize  db 'Incorrect size of vector = %d', 10, 0
        strVecElemI   db '[%d]? ', 0
        strScanInt    db '%d', 0
        strIncorElem  db 'Number entered is banned. Try another one', 10, 0
        strSuccessVec db 'Your vector:', 10, 0
        strVecElemOut db '[%d] = %d', 10, 0

        vec_size      dd 0
        num_banned    dd 0
        i             dd ?
        tmp           dd ?
        vec           rd 100

section '.code' code readable executable

start:
; 1) vector input
        push strVecSize
        call [printf]

        push vec_size
        push strScanInt
        call [scanf]

        cmp [vec_size], 0
        jl fail

        cmp [vec_size], 100
        jg fail

        jmp getNumBanned
; fail size
fail:
        push [vec_size]
        push strIncorSize
        call [printf]
        jmp finish
; continue
getNumBanned:
        push strNumBanned
        call [printf]

        push num_banned
        push strScanInt
        call [scanf]
getVector:
        xor ecx, ecx            ; ecx = 0
        mov ebx, vec            ; ebx = &vec
getVecLoop:
        mov [tmp], ebx
        cmp ecx, [vec_size]
        jge endInputVector       ; to end of loop

        ; input element
        mov [i], ecx
        push ecx
        push strVecElemI
        call [printf]

        push ebx
        push strScanInt
        call [scanf]

        checkIfElemNotBanned:
                ; check if num is banned
                mov edx, dword [ebx]

                cmp edx, [num_banned]
                jne getVecLoopCont

                push strIncorElem
                call [printf]

                mov ecx, [i]
                mov ebx, [tmp]
                jmp getVecLoop

        getVecLoopCont:
        mov ecx, [i]
        inc ecx
        mov ebx, [tmp]
        add ebx, 4
        jmp getVecLoop
endInputVector:

; 2) vector out
putVector:
        push strSuccessVec
        call [printf]

        xor ecx, ecx            ; ecx = 0
        mov ebx, vec            ; ebx = &vec
putVecLoop:
        mov [tmp], ebx
        cmp ecx, [vec_size]
        je endOutputVector      ; to end of loop
        mov [i], ecx

        ; output element
        push dword [ebx]
        push ecx
        push strVecElemOut
        call [printf]

        mov ecx, [i]
        inc ecx
        mov ebx, [tmp]
        add ebx, 4
        jmp putVecLoop
endOutputVector:

; 3) programm end
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