echo processing...
echo ping���ڣ�%date% >> pingall.txt
echo pingʱ�䣺%time% >> pingall.txt
echo.>>pingall.txt
echo ip��>>pingall.txt

for /l %%i in (1,1,255) do ping -n 1 -w 1 192.168.1.%%i |find "�ظ�" >> pingall.txt
done
echo ----------------------------------------------------------->> pingall.txt