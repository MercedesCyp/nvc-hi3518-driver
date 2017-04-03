<?php
/**
 * Created by PhpStorm.
 * User: Mojies
 * Date: 9/9/2015
 * Time: 5:37 PM
 */

class page_frame {

    /**
     * @param $iid
     * @param $iTitle
     * @param $iColumn
     * @param $iRow
     * @param $iData
     * @return string
     */
    public function pf_builtATable($iid,$iTitle,$iColumn,$iRow,$iData){

        $tTable = '<table border="1"  rules="all" align="center" width=61% ';
        if( $iid != 'NULL' ){
            $tTable .= 'id="'.$iid.'"';
        }
        $tTable .= '>';

        if( $iTitle != 'NULL' ) {
            $tTable .= '<caption>'.$iTitle.'</caption>';
        }
        for( $i=0; $i<$iRow; $i++ ){
            if( $i == 0 ) {
                $tTable .= '<tr bgcolor="#4BACC6">';
            }else{
                if( $i%2 == 0 ){
                    $tTable .= '<tr bgcolor="#D2EAF1">';
                }else{
                    $tTable .= '<tr>';
                }
            }
            for( $j=0; $j<$iColumn; $j++ ){
                if( $i == 0 ){
                    $tTable .= '<th>'.$iData[$i][$j].'</th>';
                }else{
                    $tTable .= '<td>'.$iData[$i][$j].'</td>';
                }
            }
            $tTable .= '</tr>';
        }
        $tTable .= '</table>';
        return $tTable;
    }

    /**
     * @param $iCol
     * @param $iData
     * @param $iType
     * @return string
     */
    public function pf_builtAList( $iCol, $iData, $iType ){
        $tList = '';
        $tSubList='';
        for( $i=0; $i<$iCol; $i++ ){
            $tSubList .= '<li><a target="_blank"  href='.$iData[ $i ][1].'>'.$iData[ $i ][0].'</a></li>';
        }

        switch( $iType){
            case 0: $tList.='<ul>'.$tSubList.'</ul>';break;
            case 1: $tList.='<ul type="disc">'.$tSubList.'</ul>'; break;
            case 2: $tList.='<ul type="square">'.$tSubList.'</ul>';break;
            case 3: $tList.='<ul type="circle">'.$tSubList.'</ul>';break;
            case 10:$tList.='<ol>'.$tSubList.'</ol>';break;
            case 11:$tList.='<ol type="1">'.$tSubList.'</ol>';break;
            case 12:$tList.='<ol type="a">'.$tSubList.'</ol>';break;
            case 13:$tList.='<ol type="A">'.$tSubList.'</ol>';break;
            case 14:$tList.='<ol type="i">'.$tSubList.'</ol>';break;
            case 15:$tList.='<ol type="I">'.$tSubList.'</ol>';break;
            default:$tList.='<ul>'.$tSubList.'</ul>';break;
        }
        return $tList;
    }

    public function pf_buildNote( $iTitle, $iContent ){
        $tNote='';
        $tNote .= '<div class="Block_1">';

        if( $iTitle != 'NULL' ){
            $tNote .= '<div class="Block_1_Title">'.$iTitle.'</div>';
        }else{
            $tNote .= '<div class="Block_1_Title"><br/></div>';
        }

        $tNote .='<div class="Block_1_Content">'.$iContent.'</div>';

        $tNote .= '</div >';
        return $tNote;
    }

    /**
     * @param $iContent
     * @return string
     */
    public function pf_getPage($iContent){
        header("Content-type:text/html;charset=utf8");
        $tBody_head = page_frame::pf_getBasicHead();
        $tBody_IIcon = page_frame::pf_getIConhead();
        $tBody_Content = page_frame::pf_getContent($iContent);

        $tHead  =  page_frame::pf_getPageHead();
        $tBody  =  page_frame::pf_getPageBody($tBody_head,$tBody_IIcon,$tBody_Content);

        $thtml = '<!DOCTYPE html><html>'.$tHead.$tBody.'</html>';
        return $thtml;
    }

    /**
     * @return string
     */
    private function pf_getBasicHead(){
        $basic_h = '<div id="id_pagehead"><a class="a_nocolor" href="/index.php">返回首页</a></div>';
        return$basic_h;
    }

    /**
     * @return string
     */
    private function pf_getIConhead(){

        $hsql = mysql_connect("localhost","root","724153993");
        mysql_select_db('NVDrv');
        mysql_query('set names utf8');
        $res = mysql_query('SELECT * FROM version WHERE id= (SELECT MAX(ID) FROM version)');
        $row = mysql_fetch_row($res);

        $tIcon_Table = '<tr><td>Last Version:</td><td>'.$row[1].'</td></tr>';
        $tIcon_Table .= '<tr><td>Stable Version:</td><td>'.$row[2].'</td></tr>';
        $tIcon_Table .= '<tr><td>Author:</td><td>'.$row[3].'</td></tr>';
        $tIcon_Table .= '<tr><td>ROOT MM:</td><td>'.$row[4].'</td></tr>';
        mysql_close($hsql);

        $tICon = '<div id="id_IconArea" class="curved_box">';
        $tICon.= '<div id="id_IconImg"><img src="../img/LiteralLog.png" height=139 width=300 /></div>';
        $tICon.= '<div id="id_IconText"><table style="margin-top: 10px">'.$tIcon_Table.'</table></div>';

        $tICon.= '</div>';
        return $tICon;
    }

    /**
     * @param $iContent
     * @return string
     */
    private function pf_getContent( $iContent ){
        $tContent = '<div id="main_block" class="curved_box">'.$iContent.'</div>';
        return $tContent;
    }

    /**
     * @param $iHead
     * @param $iIConHead
     * @param $iContent
     * @return string
     */
    private function pf_getPageBody($iHead,$iIConHead,$iContent){
        $tBody = '<body>'.$iHead.$iIConHead.$iContent;
        $tBody .= '<script type="text/javascript">CreateBackToTop();jf_ShowCurTime();</script>';
        $tBody .= '</body>';
        return $tBody;
    }

    /**
     * @return string
     */
    private function pf_getPageHead(){
        $head  = '<head lang="en"><meta charset="UTF-8">';
        $head .= '<title>Netview Driver Designer Document</title>';
        $head .= '<link href="../CSS/mainpage.css" rel="stylesheet" type="text/css"/>';
        $head .= '<script type="text/javascript" src="../JS/common.js"></script>';
        $head .= '</head>';
        return $head;
    }

}