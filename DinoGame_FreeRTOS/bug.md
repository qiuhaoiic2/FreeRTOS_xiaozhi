
优化后的代码，跳起来恐龙不显示，
height的变化是 0, 6, 10, 15, 18, 21, 23, 25, 25, 23, 21, 18, 15, 10, 6, 0

```c

        dino.frame ^= 1;

        uint8_t dino_y = 5 - dino_information.height / 5; // 4 -
        if (dino_y < 2)
            dino_y = 2;

        if (dino_information.height == 0)
            oled->OELD_ShowDino(oled, 0, dino_y, 15, dino_y + 1, dino_information.frame);
        else
            oled->OLED_ShowDino_Jump(oled, 0, dino_y, 15, dino_y + 1, dino_information.frame);
```

优化前的代码：
```c
		Dino_Count++;
		if(Dino_Count == 50)
		{
			Dino_Flag ^= 1;
			
			if(Dino_Jump_Key == 1)
			{
				if(Dino_Jump_Flag_Flag == 0 && Jump_FinishFlag == 0)
				{
					Dino_Jump_Flag ++;
					if(Dino_Jump_Flag == 8)
						Dino_Jump_Flag_Flag = 1;
				}
				else if(Dino_Jump_Flag_Flag == 1)
				{
					Dino_Jump_Flag --;
					if(Dino_Jump_Flag == 0)
					{
						Dino_Jump_Flag_Flag = 0;
						Jump_FinishFlag = 1;
					}
				}
			}n
			
			switch(Dio_Jump_Flag)
			{
				case 0:Height = 0; break;
				case 1:Height = 6; break;
				case 2:Height = 10;break;
				case 3:Height = 15;break;
				case 4:Height = 18;break;
				case 5:Height = 21;break;
				case 6:Height = 23;break;
				case 7:Height = 25;break;
				case 8:Height = 25;break;
			}
			
			Dino_Count = 0;
		}

        	if(Dino_Jump_Key == 0)
	{
		OLED_ShowDino(0, 5, 15, 6, Dino_Flag);																									//显示恐龙奔跑的画面
	}
	else
	{
		if(Jump_FinishFlag == 1)
			Jump_FinishFlag = 2;
    OLED_ShowDino_Jump(0, 2, 15, 6, Dino_Jump_Flag);																				//显示恐龙跳起的画面
		if(Jump_FinishFlag == 2 && Dino_Jump_Flag == 0)
		{
			Jump_FinishFlag = 0;
			Dino_Jump_Key = 0;
		}
	}
```