<?php
include ('checksession.php');
?>

<!DOCTYPE HTML>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, minimum-scale=1.0, user-scalable=yes">
    <link href="/css/vils.min.css" rel="stylesheet">
    <meta name="description" content="SMIMS VILS IoT">
    <meta name="author" content="SMIMS">
    <title>SMIMS VILS IoT - Version History</title>
  </head>

    <div class="box">
        <div class="box-header with-border">
            <h3 class="box-title"><?php echo $left_side_bar_devices_versionhistory;?></h3>
            <!-- <div class="box-tools pull-right">
                <button type="button" class="btn btn-box-tool" onclick="open_maintenance_page();return false;"><i class="fa fa-external-link"></i>
                </button>
            </div> -->
        </div>
        <!-- /.box-header -->

        <div class="box-body">
            <table class="table table-bordered">
                <thead>
                    <tr>
                        <th style="width: 10px;" class="bg-gray disabled">#</th>
                        <th class="bg-gray disabled">版本號</th>
                        <th class="bg-gray disabled">日期</th>
                        <th class="bg-gray disabled">更新內容</th>
                    </tr>
                </thead>
                <tbody>
                    <tr>
                        <td>1</td>
                        <td>V1.1</td>
                        <td>110/09/01</td>
                        <td>正式上線</td>
                    </tr>
                    <tr>
                        <td>2</td>
                        <td>V2.1</td>
                        <td>110/09/27</td>
                        <td>增加歷史火場紀錄</td>
                    </tr>
                    <tr>
                        <td>3</td>
                        <td>V3.1</td>
                        <td>110/10/12</td>
                        <td>增加待命區定位器功能</td>
                    </tr>
                </tbody>
            </table>
            <div class="btn-group">

            </div>
        </div>
        <!-- /.box-body -->
    </div>
    <!-- /.box -->


</html>
