<?php
/**
 * Created by PhpStorm.
 * User: Mojies
 * Date: 9/17/2015
 * Time: 5:15 PM
 */
include("page_frame.php");
include("pkgMySQL.php");
$buildHtml = new page_frame();
$fdMySQL = new pkgMySQL();

$mySQL_User = $fdMySQL->pf_getMysqlUser();



$pContent='
<p style="margin-top:30px;">最后修改时间：2015 年 8 月 15 日</p>
<p>修订人：Mojies</p>
<hr/>
<ul style="list-style-type: none">
    <li><a href="#Intrduce_id_h">1 说明</a>
    <ul style="list-style-type: none">
        <li><a href="#write_purpose_id_h">1.1 目的</a></li>
        <li><a href="#background_id_h">1.2 背景</a></li>
    </ul>
    </li>
    <li><a href="#Consensus_id_h">2 约定</a>
    <ul style="list-style-type: none">
        <li><a href="#DatTypeConsensus_id_h">2.1 变量类型名称约定</a></li>
        <li><a href="#DatConsensus_id_h">2.2 数据类型约定</a></li>
        <li><a href="#FunConsensus_id_h">2.3 函数类型定义</a></li>
        <li><a href="#otherConsensus_id_h">2.4 其他</a></li>
    </ul>
    </li>
    <li><a href="#drvFrame_id_h">3 驱动设计框架</a></li>
    <li><a>4 项目工程介绍</a>
        <ul>
            <li>4.1 版本说明</li>
            <li>4.2 目录介绍</li>
            <li>4.3 编译环境</li>
        </ul>
    </li>
</ul>
<hr class="hr_1"/>

<h3><a name="Intrduce_id_h"></a>1 说明</h3>
<h4><a name="write_purpose_id_h"></a>1.1 目的</h4>
<p >
    做这篇文档主要有几个目的，与项目组成员达成共识和约定，明确责任归属；做好详尽的计划，确定开发过程中各个阶段的目标，
    明确开发的重点难点有利于合理的分配资源，保证团队高质量高效的完成项目目标；记录开发方案和开发过程，
    方便测试人员等于项目相关的人员快速的了解项目的开发流程。
</p>
<p>
<p>
    关于项目则是要完成各个产品，各个平台的驱动整合。
</p>
<p style="color: red">
    项目的开发不仅仅是完成一个作品，它本身就承载着一个项目和一个开发人员从稚嫩走向成熟的过程。
</p>
<p style="color: red">
    特别要注意：本文力图说明驱动开发中的点点滴滴，逐步完善，但俗话说得好，“一图胜千言”；
    一个范例也胜千言，不要局限于文字表述，人类的智慧不是三言两语能说清楚的，看不懂的地方多研究研究代码总结出经验。
</p>

<h4><a name="background_id_h"></a>1.2 背景</h4>
<p>
    如今正值公司技术逐步走向成熟与发展的时期，为了配合高效的开发，减少不必要的资源的浪费，在驱动方面不再运用以前零散，
    无计划的开发方案 ，替换而来的是相对比较集中，既可提高开发速率，简单易懂又方便维护的方案。因为所有的摄像机设备软件
    基本上都是基于 linux 开发来的，因此在代码方便，各个产品的相似性比较多，这样实现起来也会比较容易。
</p>
<p>
    本次需要进行整合驱动的产品有‘即视通二’，‘即视通三系列’，主要开发平台也是基于由两个核心两个芯片（HI3507R，HI3518）
    提供商提供的SDK。代码用C语言实现，符合 Linux 驱动接口规范。
</p>

<h3><a name="Consensus_id_h"></a>2 约定</h3>
<p>

</p>
<h4><a name="DatTypeConsensus_id_h"></a>2.1 变量类型名称约定</h4>
<ol>
    <li>uint8 --> unsigned char</li>
    <li>int8 --> char</li>
    <li>uint16 --> unsigned short</li>
    <li>int16 --> short</li>
    <li>uint32 --> unsigned long</li>
    <li>int32 --> long</li>
    <li>int --> int</li>
    <li>unsigned int --> unsigned int</li>
    <li>前缀--m 结构体类型变量类型</li>
    <li>前缀--e 枚举变量类型</li>
    <li>前缀--u 联合变量类型（注意：以上三个前缀是修饰数据类型的，通常只与前缀--p 做搭配）</li>
</ol>

<h4><a name="DatConsensus_id_h"></a>2.2 数据类型约定</h4>
<ol>
    <li>前缀--c 已 const 关键字修饰的常量</li>
    <li>前缀--r 已 restrict 关键字修饰的指针</li>
    <li>前缀--v 已 volatile 关键字修饰的变量</li>
    <li>前缀--s 以static 关键字修饰的局部变量</li>
    <li>前缀--t 位于函数内的局部变量</li>
    <li>前缀--i 函数的传入参数</li>
    <li>前缀--n 函数用于返回的变量</li>
    <li>前缀--g 全局变量，全局有效，外部以extern关键字调用</li>
    <li>前缀--p 最指针类型（无论是几级指针都只用一个关键字修饰）</li>
    <li>前缀--a 数组和结构体成员</li>
</ol>

<h4><a name="FunConsensus_id_h"></a>2.3 函数类型定义</h4>
<ol>
    <li>前缀--gf 全局函数</li>
    <li>前缀--sf 局部函数</li>
    <li>前缀--pf 函数指针(注意函数指针也相当一个变量，也可能被 const restrict volatile static等函数修饰，但都用 pf 修饰)</li>
    <li>前缀--af 函数变量(通常申明在结构体中，但也有少数申明在外部)</li>
</ol>

<h4><a name="otherConsensus_id_h"></a>2.4 其他</h4>
<ol>
    <li>前缀--DC_ 宏类型，定义一个数值类型</li>
    <li>前缀--DF_ 宏类型，定义一串方法</li>
    <li>所有的类型，变量（除宏）经过前缀修饰后接修饰变量具体作用的名称，名称要简短，如果单词过长影响阅读可适当的简写一个单词，
        并做好注释，（如：temperature 简写成 temp，// temp 为 temperature 的简写）首字母大写（如：afAbcDefGhi），
        如果有缩写则用下划线将缩写隔离（如:afPTZ_GetData,sOpt_PTZ_Value）</li>
    <li>函数中用于局部用于循环体计数的变量一般不需要前缀修饰，按顺序优先使用 _i,_j,_k,_a,_b,_c（如需要也可加 Count_ 做前缀）</li>
    <li>文档布局(依照以下的顺序，从上至下)
        <ol type="a">
            <li>文档说明(开发日期，开发人员，文档主要提供的方法说明)</li>
            <li>头文件（编译工具库头文件——平台库头文件——本工程头文件——远程工程头文件）如：代码块 Block_1</li>
            <li>声明调用的函数（外部调用的函数——本地调用的函数（本地调用的函数一般是静态函数，调用顺序为被调用的排头，
                如果有全局函数，那么全局函数排在最尾））如：代码块 Block_2</li>
            <li>变量声明（外部变量——本地变量——全局变量）如：代码块 Block_3</li>
            <li>函数实现（全局函数——本地函数）</li>
        </ol>
    </li>
    <li>函数的实现实现格式如：如：代码块 Block_4</li>
    <li>限定函数域的大括号必须另起一行,但是限定循环语句函数域的大括号需要更随语句本身（在 代码块 Block_3 也有体现）</li>
</ol>


<div class="CodeBlockTitle">代码块 Block_1</div>
<pre >
    //==============================================================================
    // C
    #include &lt;stdio.h&gt;
    // Linux
    #include &lt;linux.h&gt;
    // local
    #include "string.h"
    // remote
    #include "../Tool/string.h"
</pre>
<div class="CodeBlockTitle">代码块 Block_2</div>
<pre >
    //==============================================================================
    //extern
    extern void gfExternFunction(void);

    //local
    static void sfStaticLocalFunction1(void);
    static void sfStaticLocalFunctino2(void);   // sfStaticLocalFunctino2() 调用过 sfStaticLocalFunction1()

    //global
    void gflocalFunction(void);
</pre>
<div class="CodeBlockTitle">代码块 Block_3</div>
<pre >
    //==============================================================================
    //extern
    extern (void)(*pfSomeThing_CallBack)(void)

    //local
    static uint8 sLocalStatus;

    //global
    uint8 gGlobalVariable;

</pre>
<div class="CodeBlockTitle">代码块 Block_4</div>
<pre >
    //==============================================================================
    //Global
    //---------- ---------- ---------- ----------
    (this is first global function)/*
    @introduction:
        This is introduction of this function.
    @parameter:
        iCmd:
            Next lines shows whats meaning of input value
            0: open light
            1: close light
            others： return -1
    @return：
        -1: Error input
    */
    void gfLED_SwitchStatus(uint8 a,uint8 b)
    {
        if(a>b){

        }

        do{

        }while(a>b);

        while(1){

        }
    }
    // 3 empty lines
    //---------- ---------- ---------- ----------
    // second global function
    // 5 empty lines
    //------------------------------------------------------------------------------
    // Local
    //---------- ---------- ---------- ----------
    // first local function
    // 3 empty lines
    //---------- ---------- ---------- ----------
    // second local function
    // 10 empty lines
    //==============================================================================
    // Others
</pre>


<h3><a name="drvFrame_id_h"></a>3. 驱动设计框架</h3>
<p>
图3.1 描述的是整个驱动程序的框架，整个分为 linux 驱动接口层，消息处理层，消息管理层，功能层，硬件接口层，产品配置单元。
    接下来我将针对各个层的功能和层与层之间的关系一一介绍。
</p>
<p>
上图中 onStart,onStop,optSelect,OptRead,optWrite 构成与 linux 驱动接口层，在驱动被加载的时候，内核会调用此层中的
    __Init() 函数对整个驱动做初始化，而在这个函数里面则会根据事先的配置，初始化好硬件，事件调度工具，消息内存管理，以及根据加载驱动的时候传入的
    Chip 和 Pro 参数来确定加载和初始化哪些功能模块。在卸载驱动的时候则会依次卸载初始化好的这些模块。当然，这个模块中也提供了
    一些必要的与应用层通信的内核驱动接口，这些接口会处理记录 open close read write check 的相关信息，另一方面也对应用层的并发访问，
    超标访问做相应的上锁和限制。
</p>
<p>
第二个部分是消息处理层，这层主要处理做两个事。一件事根据之前初始化时分析出来的机型来挂载相应的功能模块。第二个是响应自应用层的控制事务，
    包括检查消息的合法性，更具消息类型来执行相应的操作。执行这些操作的接口主要由消息管理层和功能层提供，
    最后生成的响应消息会通过消息管理层提供的 API 放入管理的内存池中。
</p>
<p>
消息管理层主要负责管理消息的暂存，如果消息处理层通知消息管理模块有人订阅了上报消息，则功能层产生的上报消息都将
    缓存在这里，消息处理模块的响应消息也将缓存在这里，它还向内核驱动接口层提供消息以及订阅者的读写，删除等方法。如果在驱动运行时
    出现内存错误的信息，大多也是这个层内存溢出导致的。
</p>
<p>
功能层，包括的驱动模块有按键、指示灯、LDR、红外灯、IRCut、PTZ...这里面主要是向消息处理模块提供控制各个硬件模块的接口，
    但在这个模块中它只实现操作硬件模块的逻辑，因为不同的芯片，所涉及到得硬件接口会不相同，
    所以它真正去控制硬件模块还要通过下面的硬件抽象层提供的接口。
    但要注意的是，由于这个层里面的控制方法为所有机型共有，因此，在添加新机型的时候，为了避免不正当的操作影响别的机型功能，
    我们需要将新添加的功能代码先放置在机型配置单元的文档中，待新驱动功能测试之后再添加到这个层当中。
    一般提供给消息处理层的接口主要有模块的初始化，收尾，Set，Get，上报等。
</p>
<p>
硬件抽象层，这一层主要是将各个芯片的硬件控制方式抽象出来，像 GPIO 的输入输出，获取状态，置高置低，中断设置，中断方法注册；
    PWM Timer ADC 等 模块的设置和获取状态信息，IIC，UART协议的实现。当然还提供事件注册接口。
</p>
<p>
配置单元，这个层一方面向 linux 驱动接口层提供判断加载机型的合法性接口；另一方面存放各个机型的配置信息。
</p>
<div style="text-align: center">
    <div class="imgBlockTitle">（图3.1 驱动整合框图）</div>
    <img src="../DATA/Driver/驱动框架设计.jpg" height=90% width=90% >
</div>

<h3><a name="prjIn_id_h"></a>4 项目工程介绍</h3>
<p>

</p>
<h4><a name="prjVer_id_h"></a>4.1 版本说明</h4>
<p>
如果有框架上面或者有较大的更新，或者有较大的
<p>

</p>
<h4><a name="prjVer_id_h"></a>4.2 目录介绍</h4>
<h4><a name="prjVer_id_h"></a>4.3 编译环境</h4>

';



$html=$buildHtml->pf_getPage($pContent);
echo $html;