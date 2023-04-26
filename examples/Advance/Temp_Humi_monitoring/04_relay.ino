

void relayLoop(void)
{
    if (TEMP >= setVar1 && setVar1 > 0)
    {
        iMi3.relayHi(R1);
    }
    else
    {
        iMi3.relayLo(R1);
    }

    if (TEMP >= setVar2 && setVar2 > 0)
    {
        iMi3.relayHi(R2);
    }
    else
    {
        iMi3.relayLo(R2);
    }
    if (HUMI >= setVar3 && setVar3 > 0)
    {
        iMi3.relayHi(R3);
    }
    else
    {
        iMi3.relayLo(R3);
    }
    if (HUMI >= setVar4 && setVar4 > 0)
    {
        iMi3.relayHi(R4);
    }
    else
    {
        iMi3.relayLo(R4);
    }
}
