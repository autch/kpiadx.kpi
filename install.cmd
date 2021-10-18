set DEST=%2
if not exist %DEST%\nul mkdir %DEST%
copy .\%1\kpiadx.kpi %DEST% /y
copy .\%1\*.pdb %DEST% /y
