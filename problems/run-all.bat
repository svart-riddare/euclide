@echo off

set timeout=60

set problems=
set problems=%problems%;no-captures.txt
set problems=%problems%;single-capture.txt
set problems=%problems%;natch-test-problems.txt
set problems=%problems%;bichromatic.txt
set problems=%problems%;edupuis.txt

set euclide=..\euclide-console\bin\euclide-console.exe

for %%p in (%problems%) do (
    %euclide% %%p --timeout=%timeout%
)
