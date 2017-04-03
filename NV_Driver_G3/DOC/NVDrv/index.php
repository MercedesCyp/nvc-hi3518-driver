<?php
/**
 * Created by PhpStorm.
 * User: Mojies
 * Date: 9/9/2015
 * Time: 3:50 PM
 */
include("PHP/page_frame.php");
$buildHtml = new page_frame();

$pContent='
    <p>本说明文档主要介绍驱动层与硬件应用层的规范，部分代码功能的说明，驱动验证程序的操作说明，驱动测试报告。也包括驱动更
    新日志，以及常见的故障处理解答。</p>
    <p>如果是第一次阅读此文档，建议先查阅 <code>READ ME FIRST</code> 该文档，这个文档是用来记录开发驱动时积累下来的一些经验，
    如果有新的想法，或者对驱动做进一步详细的开发说明，最好记录在此文档中。目前该文档介绍了驱动在整个产品中的位置，
    以及在开发初期要具备的一些基本技能，要注意的事项等。同时也会介绍驱动项目中所涉及到得一些资料，以及如何搭建驱动开发环境。
    同样的，如果你在开发过程中遇到得问题，可以先看看这一个文档，可能答案就在其中</p>
    <p>驱动支持机型：这个文档是由 <code>php</code> 文档索引 <code>mysql</code> 数据库之后生成的，所以如果要删除或者修改记录条目，
    需要手动登陆到 mysql 数据库，手动删除，如果要填加条目，直接可在屏幕下方添加提交。目前的数据库在哪里，以及如何登陆迁移，请移步
    <code>READ ME FIRST</code> 。</p>
    <p>硬件接口协议：这个文档定义了与一些机型相关的模块的驱动方式，但是从门铃机型开始，硬件方面会提供相关的定义文档
    （注意注意查阅 <code>原理图</code> 部分)，因此后续更新至需要在数据库的 <code>document</code>
    表中添加与机型相关文档的索引即可。</p>
    <p>应用层与驱动层的通信协议：驱动层与应用层通信的通信协议，这个文件应处于持续的更新当中。</p>
    <p>驱动程序开发说明</p>

    <ul>
        <li><a href="PHP/README.php">READ ME FIRST</a></li>
        <li><a href="PHP/drv_support.php">驱动支持机型</a></li>
        <li><a href="PHP/hardware.php">硬件接口协议</a></li>
        <li><a href="PHP/AppToDrvProtacal.php">应用层与驱动层的通信协议</a></li>
        <li><a href="PHP/DrvDevelopDoc.php">驱动程序开发说明</a></li>

    </ul>
    <br/><br/><br/><br/><br/><br/><br/>
';


$html=$buildHtml->pf_getPage($pContent);
echo $html;

