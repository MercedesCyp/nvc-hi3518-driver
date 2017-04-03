<?php
/**
 * Created by PhpStorm.
 * User: Mojies
 * Date: 9/8/2015
 * Time: 5:23 PM
 */

include("../PHP/page_frame.php");
include("pkgMySQL.php");
$buildHtml = new page_frame();
$fdMySQL = new pkgMySQL();

$mySQL_User = $fdMySQL->pf_getMysqlUser();


$pContent  .='<div class="Head_Style_2" style="text-align: center;margin-top:30px; ">驱动支持信息</div>';
$pContent  .='<hr class="hr_1"/><br/><br/>';
$pContent  .='<table border="1" align="center">';
$pContent  .='<tr><th>GENERATION</th><th>机型</th><th>芯片类型</th><th>驱动机型ID</th><th>驱动芯片ID</th><th>备注</th></tr>';
$mysql_conn = mysql_connect("localhost",$mySQL_User[0],$mySQL_User[1]);
mysql_select_db('NVDrv');
mysql_query('set names utf8');
$res = mysql_query('SELECT * FROM product');
while( $row = mysql_fetch_row($res) ){
    $pContent  .='<tr><td>'.$row[1].'</td><td>'.$row[2].'</td><td>'.$row[3].'</td><td>'.$row[4].'</td><td>'.$row[5].'</td><td>'.$row[7].'</td></tr>';
}
$pContent  .='</table>';
$pContent  .='<div class="Block_1"><p>注意：</p></div>';

$pContent  .='
<div class="Head_Style_2" style="text-align: center;margin-top:30px; ">添加新支持机型</div>
<hr class="hr_1"/>
<br/><br/>

<div class="Block_Center">
<p style="color: red">下面的表单可以很简便的将一个新的机型的信息添加至服务器，但是为了防止误删，正对数据库中存在的机型信息的删除和修改操作不在此处进行，
请手动登陆 Mysql 进行修改。</p>
<p>下面将对各个提交的字段做详细解释：<code>GENERATION</code> ，每一次软件与硬件经历一次大的框架的变动，将会产生一个新的版本，
而这个字段正是描述当前机型所利用的版本。<code>机型</code>，产品的实际名称。<code>芯片类型</code>，核心处理器的名称。
<code>驱动机型 ID</code>，机器识别数字比识别字符串的能力强，因此在驱动中每一个机型对应一个机型的 ID（产品 ID ），
而这个字段即代表机型在驱动中对应得 ID 值。<code>驱动芯片 ID</code>，与驱动机型 ID 相同，本字段是代表机型利用的微处理器芯片在驱动中对应的 ID。
<code>支持模块</code>，本机型支持的硬件模块，至于想要查看所有机型涉及到那些模块，可以点击首页中 <code>硬件接口协议</code> 连接进入查看。
<code>备注</code>，对该机型做外观上，价格上作更加的详细描述，用以方便开发人员的辨认。</p>
</div>
<div class="Block_Center"><form action="drv_support_s.php" method="post"><fieldset>
<legend style="text-align: center">添加支持机型</legend>
<table>
<tr><td>GENERATION：</td><td><input type="text"      name="Generation"></td></tr>
<tr><td>机型：</td><td><input type="text"            name="Pro_name"></td></tr>
<tr><td>芯片类型：</td><td><input type="text"        name="Chip_type"></td></tr>
<tr><td>驱动机型 ID：</td><td><input type="text"     name="Pro_id"></td></tr>
<tr><td>驱动芯片 ID：</td><td><input type="text"     name="Chip_id"></td></tr>
<tr><td>支持模块：</td><td><textarea cols="50" rows="6" name="Supp_mod"></textarea></td></tr>
<tr><td>备注：</td><td><textarea cols="50" rows="6"  name="Remark"></textarea></td></tr>
<tr><td></td><td><input type="submit"                value="提交"></td></tr>

</table>
</fieldset></form></div>
';


mysql_close($mysql_conn);

$html=$buildHtml->pf_getPage($pContent);
echo $html;
