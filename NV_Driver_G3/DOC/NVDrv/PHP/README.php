<?php
/**
 * Created by PhpStorm.
 * User: Mojies
 * Date: 9/17/2015
 * Time: 5:45 PM
 */
include("page_frame.php");
include("pkgMySQL.php");
$buildHtml = new page_frame();
$fdMySQL = new pkgMySQL();

$mySQL_User = $fdMySQL->pf_getMysqlUser();


$tTitle = '文档说明';
$tContent = '
<p>在 /mnt 目录下有一个 EXCHANGE 目录，此目录同时在 /home/maj 目录下有备份，这个目录为驱动项目的交接目录，重要资料都基本
上在这个目录中，请注意误删！</p>
<p>在多废话几句，3507R 为即视通二（B14）中用到的华为海思生产的视频芯片，3518 同样也是有华为海思生产的，在即视通三的 D04 和 D11
机型中用到的是 3518C 剩下的几个机型用的是 3518E</p>
<p>下面，我将对该目录下的各个文档，目录做一一解释：目录 SDK 下存放了三个文档，<code>Hi3511_DMS_V110R001C01B123_CN.rar</code> 是用于我们即视通二 3507 的开发工具，
<code>Hi3518_SDK_V1.0.9.0.tgz</code>是用于当前即视通三中 3518C，3518E，两款芯片的开发工具，解压
<code>Hi3511_DMS_V110R001C01B123_CN.rar</code> 时需要解压密码，解压密码在 <code>SDK_3511解压密码</code>文档中，也就是
<code>Hi3511e2W2</code>。首先，解压<code>Hi3511_DMS_V110R001C01B123_CN.rar</code>完成后，会在
<code>./software/Hi3511 VSSDK_V1.1.2.3</code>目录下找到一个叫<code>Hi3511_VSSDK_V1.1.2.3 安装以及升级使用说明.txt</code>
的文件，这个文件里面会介绍怎么安装海思 3507R （我们摄像机中用到）的开发工具。3518 同样也有这个文件去安装，目录为
<code>software/board/documents_cn/Hi3518 SDK 安装以及升级使用说明.txt</code>。</p>
<p>接下来，介绍一下在开发过程中可能用到的一些工具。markdownpad，在 GitHub 上，大多文档都采用 markdown 格式，因此，在 windows 上
这是一个编写 makedown 文档的一个不错的软件。lamp， 这是一个软件包的统称，linux+apache+mysql+php，因为驱动的说明文档大多都是由
html 和 php 写的，中间有些地方还用到了数据库，因此，建议先把这个搭建起来。如果你要修改，完善驱动开发文档，可能需要一些基本的
前端方面的知识，推荐一个 php 文档的编辑器 phpstorm。</p>
<p><code>NV_Driver_G3</code>,这个文件夹即是驱动的代码，该文件夹下有一个<code>/DOC/NVDrv</code>的文件夹，这就是驱动开发说明文档的所在目录，
你可以直接在该目录下，拷贝到你想要的目录，然后把那个目录设置为你虚拟主机的目录，如果你局域网打开了，你就可以再浏览器中直接输入
你的 ip 地址，便可访问到该文档内容。但前提是要具备搭建虚拟主机的能力。直接看文档中的内容，可能会发现有些糟糕，所以建议还是把虚拟主机
搭建起来。</p>
<p>上边介绍到，在<code>NV_Driver_G3/DOC/NVDrv</code>目录下有驱动开发说明文档，该文档中有些内容是直接从数据库中获取的，数据库的备份在
<code>SQL</code>目录下，基于Mysql的，里面的数据不多，开发还不够完善，很多文档中的数据迁移还不够完善，而且，为了规范化，系统化，
更加高效，便捷的管理这个工程，建议还是把数据库建立起来，然后在把该文件下的数据迁移到你的数据库的保存目录下。</p>
<p>在驱动代码目录下还有几个文件夹,<code>APP</code>目录下存放的是驱动的测试程序，这只是功能性测试，而且是 很老的一个测试程序，
 很可能不能完全测试出驱动程序的漏洞，在开发驱动程序后，最好跟进这个驱动程序。当然，在以后为了更好的保证软件的稳定性，
 还要利用一些工具，如<code> C-Unit</code>,进行一些结构性测试。<code>history</code>目录下存放的是过期的，淘汰的一些程序文档，
 在时间的推移过程中，有项目会被作废，有项目会被抛弃，但跟这些项目有关的文件，老旧的代码应该保存下来，而这个文件就是存放过期的
 ，作废的，代码文档，或者说明文档</p>
<p>如果想了解代码中的文档架构，请移步至<code>/NV_Driver_G3/Readme</code>文件。</p>
';
$tBlock = $buildHtml->pf_buildNote($tTitle,$tContent);
$pContent.=$tBlock;



$tTitle = '开发环境搭建';
$tContent = '
<p>这里来讲一下开发环境的搭建。开发环境个人认为可分为这几个部分，一个是硬件上，怎么样的计算机环境，怎么样的网络环境，
能让你与整个团队的工作效率达到饱和，这是要根据个人喜好来设置的。然后是软件环境，如共享空间，代码编辑器，软件测试工具，之类的，
这些都安装好了之后就可以安装海思提供的 SDK 了。</p>
<p>下面粗略地介绍一下我的开发环境。我用公司的电脑装上了一个 <code>ubuntu 10.04 i386 server</code> 的操纵系统，
因为基本上是通过 <code>ssh</code> 远程登录来进行开发，因此我并不需要安装桌面版。当然平时要在 linux 系统和 windows 系统之间传送文件，
因此在 windows 上设置几个文件共享目录，和在 linux 上安装 <code>samba</code>,<code>nfs</code>服务是必要的。当然，
你也可以在 windows 利用软件 <code>Filezilla</code> 通过 sftp 来实现文件的上传下载，但个人认为，搭建好网络文件共享服务器会更加方便。
然后在你的系统上装上搭建开发环境中或者平时进行程序开发时候的开发工具，如：gcc，g++，make，make-guiule,libncurses5-dev,automaker，
autoconf，unzip,unrrar,ia32-lib……。安装完上述软件之后就可以开始安装海思的 SDK 了。但在安装的时候要注意的一点，
海思安装脚本中 <code>#!/bin/sh</code> 所指的脚本工具是 <code>bash</code> 但 ubuntu 的系统 bin 目录下的 sh 程序一般是软连接到 dash 上的，
这并不是说 dash 比较强大，而是 dash 比 bash 精简性能比较高，事实上 dash 恰恰是由 bash 简化而来，而我们开发这个程序并不需要这么高的要求，
因此你可以毫不犹豫的执行下面这条语句来将 dash 替换成 bash。</p>
<pre> rm /bin/sh && ln -s /bin/bash /bin/sh</pre>
<p>然后我们要做的就是安装海思的 SDK ，安装步骤在海思提供的 SDK 的软件说明文档里有，具体见上一章。要注意的是，安装完成交叉编译工具之后，
要重启一次系统，我记得的是虽然安装的时候已经把开发环境变量设置到 <code>/etc/profile</code> 文件中，但是并不存在当前的 <code>PATH</code> 环境变量中，
因此要不就执行 <coed>resource /etc/profile</coed>，要不就重启。之前也碰到过有些操作系统启动时并不会执行 <code>/etc/profile</code> 这个文件，
如果重启后发现交叉编译工具目录并不在 <code>PATH</code> 环境变量中，那么此时你可以检查一下是否发生上上边我碰到的这个问题，
如果是上面那个问题，你可以试着直接把交叉编译工具的目录添加到 <code>/etc/environment</code> 的末尾，然后重启电脑。
有时候也会碰到操作系统是 64 位，而交叉编译工具只能运行在 32 位系统下的问题，这个也可以注意一下。</p>
<p>安装完 SDK 之后，还不能马上编译我们的驱动，因为编译驱动时我们要用到目标系统的内核资源，因此，下一步，就是编译内核，编译内核请参考下面两个文件，
<code>/software/board/documents_cn//Hi3518A Hi3518C Hi3518E Hi3516C Linux开发环境 用户指南.pdf</code> 和
<code>/Hi3511 DMS V110R001C01B123中 CN/中文版/software/Doc/Hi3511／Hi3512 Linux开发环境用户指南.pdf </code>，
按照里面写的配置内核就行了，3511 用的是 mini 版，3518 也用的是 mini 版。安装完成后我们还要设置即视通三开发环境，
用 <code>NVC_HISDK3518</code> 指定海思 3518 的 SDK 安装路径，用 <code>NVC_HISDK3507</code> 指定海思 3507 SDK 的安装路径，
注意：这里的安装路径是 SDK 的源码路径，并不是 交叉编译工具的路径。然后，用 <code>NVC_BASE</code> 存放摄像机程序，驱动的安装路径。
可以运行下面的程序:（注意，其中的 path 是值安装路径，紧接 echo 后的是 >> 而不是 > 不然一不小心，profile 中的数据都被覆盖了）
</p>
<pre>
sudo echo export NVC_HISDK3518="path1" >> /etc/profile
sudo echo export NVC_HISDK3507="path2" >> /etc/profile
sudo echo export NVC_BASE="path3" >> /etc/profile
sudo reboot;
</pre>
<p>执行完上面的那些步骤之后就可以正式的进行项目的开发了。</p>
';
$tBlock = $buildHtml->pf_buildNote($tTitle,$tContent);
$pContent.=$tBlock;


$tTitle = 'WEB 本文说明以及文档运行环境';
$tContent = '
<p>目前该文档运行于公司内网 ip 为 192.168.0.77 的一台计算机上，计算机上运行的是 LAMP，查看本网页时最好用 Chrome 浏览器，
使用其他浏览器可能导致排版错误。该计算机可通过 ssh 登录，登录用户名为 (maj), 密码 (123)， 管理员密码 (724153993)。</p>
<p>该文档放置于 <code>/WEB/NVDrv</code> 目录下，解析域名为 www.nvdrv.com， 端口为 80。</p>
<p>本文档用到的 Mysql 库存在
<code>/var/lib/mysql/NVDrv/</code> 目录下可，通过 SSH 登录之后访问，也可访问 192.168.0.77:8080 ，通过 PhpMyAdmin 访问。
登录密码为 (724153993)</p>
<p>关于对网页中各个文档的大体介绍，存在于首页中，如果想查看详细的内容，请访问相关页面。目前文档还处于编写初期，
框架还不完善，还有大部分数据的提取和组织未得到简化，如果有时间应该尽量完善。</p>
';
$tBlock = $buildHtml->pf_buildNote($tTitle,$tContent);
$pContent.=$tBlock;



$tTitle = '开发说明文档介绍';
$tContent = '

';
$tBlock = $buildHtml->pf_buildNote($tTitle,$tContent);
$pContent.=$tBlock;


$html=$buildHtml->pf_getPage($pContent);
echo $html;

