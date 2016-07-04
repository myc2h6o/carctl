echo processing...
echo ping日期：%date% >> pingall.txt
echo ping时间：%time% >> pingall.txt
echo.>>pingall.txt
echo ip：>>pingall.txt

for /l %%i in (1,1,255) do ping -n 1 -w 1 192.168.1.%%i |find "回复" >> pingall.txt
done
echo ----------------------------------------------------------->> pingall.txt