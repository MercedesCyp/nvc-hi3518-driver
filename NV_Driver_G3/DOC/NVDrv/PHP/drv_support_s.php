<?php
/**
 * Created by PhpStorm.
 * User: Mojies
 * Date: 10/8/2015
 * Time: 12:02 PM
 */

$vGeneration = $_POST[Generation];
$vProName    = $_POST[Pro_name];
$vChip_type  = $_POST[Chip_type];
$vPro_id     = $_POST[Pro_id];
$vChip_id    = $_POST[Chip_id];
$vSupp_mod   = $_POST[Supp_mod];
$vRemark     = $_POST[Remark];



include("pkgMySQL.php");
$fdMySQL = new pkgMySQL();
$fdMySQL->pf_InsertASupportPro($_POST);

