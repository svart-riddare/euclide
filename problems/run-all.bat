@echo off

set timeout=60

set problems=
set problems=%problems%;no-captures.txt
set problems=%problems%;single-capture.txt

set euclide=..\euclide-console\bin\euclide-console.exe

for %%p in (%problems%) do (
    %euclide% %%p --timeout=%timeout%
)
