@ECHO OFF
set argc=0
for %%x in (%*) do Set /A argc+=1
set IGNORE="/.vs,/Debug"
IF %argc% EQU 0 (
curl https://www.gitignore.io/api/%IGNORE%
) ELSE (
curl https://www.gitignore.io/api/%1
)

