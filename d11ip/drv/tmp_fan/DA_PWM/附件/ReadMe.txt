1.	Hi3518 提供3路PWM脉宽调制信号输出
						26bit
	支持				Max	1.5MHz Min 0.045Hz
	高电平个数 			26Bit可配置
	内部10bit计数器
	
2.	配置步骤：
	a.	通过计算得到需要的周期数，和高电平的个数。
	b.	禁用PWM输出		PWM_CTL 写0
	c.	将对应数据写入PWM0——CFG0 PWM0——CFG1 PWM0——CFG2
	d.	使能PWM输出
	e.	回读PWM0——STATE0，PWM0——STATE0，PWM0——STATE0的数据，确认配置生效

3.		
	
	
	
PWM0 内部晶振为 3M Hz
我们计算输出的频率大约为 10K
那么电路图的各项参数应如下附件中 图1.jpg 所示


图1 参数说明
频率 			10KHz
IO口电压		3.3v
驱动最大电压 	5v




