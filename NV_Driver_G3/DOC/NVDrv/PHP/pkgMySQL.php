<?php
/**
 * Created by PhpStorm.
 * User: Mojies
 * Date: 9/17/2015
 * Time: 2:22 PM
 */

class pkgMySQL {

    private   $MySql_User='root';
    private   $MySql_Passwd='724153993';
    private   $MySql_ColNum;

    /**
     * @return mixed
     */
    public function pf_getMysqlUser(){
        $tS_Mysql[0]    = $this->MySql_User;
        $tS_Mysql[1]    = $this->MySql_Passwd;
        return $tS_Mysql;
    }

    public function pf_InsertASupportPro( $Data ){
        $tConn = mysql_connect("localhost", $this->MySql_User, $this->MySql_Passwd);
        mysql_select_db('NVDrv');
        mysql_query('set names utf8');
        $sMS_QueryInsert  = 'INSERT INTO product(generation, h_type, h_core, s_type_id, s_core_id, supp_module, remark)';
        $sMS_QueryInsert .= ' VALUES ("'.$Data[Generation].'","'.$Data[Pro_name].'","'.$Data[Chip_type].'","'.$Data[Pro_id].'","'.$Data[Chip_id].'","'.$Data[Supp_mod].'","'.$Data[Remark].'")';
        mysql_query( $sMS_QueryInsert );
        mysql_close($tConn);
    }

    /**
     * @param $iDoc_Class
     * @return mixed
     */
    public function pf_getDocAPath( $iDoc_Class ){
        $tConn = mysql_connect("localhost", $this->MySql_User, $this->MySql_Passwd);
        mysql_select_db('NVDrv');
        mysql_query('set names utf8');

        $tSchStr = 'SELECT doc_name,doc_path FROM document WHERE doc_class=';
        $tSchStr.='"'.$iDoc_Class.'"';

        $res = mysql_query( $tSchStr );
        $tCount_i=0;
        $tDP = [];
        while( $row = mysql_fetch_row($res) ){
            $tDP[ $tCount_i ] = $row;
            $tCount_i++;
        }
        $this->MySql_ColNum = $tCount_i;
        mysql_close( $tConn );
        return $tDP;
    }

    /**
     * @return array
     */
    public function pf_getDeviceSuppModList(){
        $tConn = mysql_connect("localhost", $this->MySql_User, $this->MySql_Passwd);
        mysql_select_db('NVDrv');
        mysql_query('set names utf8');
        $res = mysql_query('SELECT h_type,supp_module FROM product');

        $table_date[0][0]='机型';
        $table_date[0][1]='硬件模块';
        $Count_i = 1;

        while( $row = mysql_fetch_row($res) ){
            $table_date[ $Count_i ][0]  =$row[0];
            $table_date[ $Count_i ][1]  =$row[1];
            $Count_i++;
        }
        mysql_close($tConn);
        $this->MySql_ColNum = $Count_i;
        return $table_date;
    }



    public  function pf_getColNum(){
        return $this->MySql_ColNum;
    }


}