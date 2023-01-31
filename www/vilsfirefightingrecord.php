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
    <title><?php echo $firefighting_record; ?></title>
  </head>

  <div class="modal fade" id="modal-askcloseaction">
    <div class="modal-dialog">
      <div class="modal-content">
        <div class="modal-header">
          <button type="button" class="close" data-dismiss="modal" aria-label="Close">
            <span aria-hidden="true">&times;</span></button>
          <h4 class="modal-title">確認任務結束, 並匯入氣瓶使用資料?</h4>
        </div>
        <div class="modal-body">

            <div style="margin-top:10px;">選項1:
                <button class="btn btn-info" onclick="continue_case_click();return false;">不要任務結束!</button>
            </div>

            <div style="margin-top:10px;">選項2:
                <button class="btn btn-warning" onclick="end_case_and_save_airrecord_click();return false;">任務結束並匯入氣瓶使用資料!</button>
            </div>

            <div style="margin-top:10px;">選項3:
                <button class="btn btn-success" onclick="end_case_click();return false;">僅任務結束!</button>
            </div>

        </div>
        <div class="modal-footer">
          <button type="button" class="btn btn-default" data-dismiss="modal">Close</button>
        </div>
      </div>
      <!-- /.modal-content -->
    </div>
    <!-- /.modal-dialog -->
  </div>
  <!-- /.modal -->

<div class="row">
    <div class="col-md-12">

        <div class="box box-primary">
            <div class="box-header with-border">
                <h3 class="box-title"><?php echo $firefighting_record; ?></h3>
                <div class="box-tools pull-right">
                    <div class="btn-group">
                        <div id="connection_status"></div>
                    </div>
                    <button type="button" class="btn btn-box-tool" onclick="open_firefighting_record_page();return false;"><i class="fa fa-external-link"></i>
                    </button>
                </div>
            </div>

            <div class="box-body">

                <div class="row">
                    <div class="col-md-6">
                        <div class="box box-success">
                            <div class="box-body">
                                <div class="btn-group">
                                    <button type="button" class="btn btn-default btn-sm dropdown-toggle" data-toggle="dropdown" style="font-size:20px;" id="cur_address">火場<span class="caret"></span></button>
                                    <ul class="dropdown-menu" id="dropdown_addresses">
                                    </ul>
                                </div>

                                <div id="record_starttime">火場記錄開始時間:</div>
                                <button type="submit" class="btn btn-danger pull-left" onclick="closeNewFireFightingbtn_click();return false;" id="closecasebutton">結束此項任務?</button>
                            </div>
                        </div>
                    </div>
                    <!-- /.col-md-6 -->

                    <div class="col-md-6">

                        <div class="box box-primary">
                            <div class="box-body">
                                <div class="btn-group">
                                    <div id="Address_longitude">經度:</div>
                                </div>
                                <div class="btn-group">
                                    <div id="Address_latitude">緯度:</div>
                                </div>

                                <div id="mapid" style="height:200px;"> </div>
                            </div>
                        </div>
                    </div>
                    <!-- /.col-md-6 -->
                </div>
                <!-- /.row -->

                <div class="row">

                    <div class="col-md-6">

                        <div class="box box-danger">
                            <div class="box-header with-border">
                                <div class="col-md-2">
                                    <h3 class="box-title">管制區</h3>
                                </div>

                                <div class="col-md-4">
                                    <div class="btn-group">
                                        <button type="button" class="btn btn-default">選擇進入管制器</button>
                                        <button type="button" class="btn btn-default dropdown-toggle" data-toggle="dropdown">
                                            <span class="caret"></span>
                                            <span class="sr-only">Toggle Dropdown</span>
                                        </button>
                                        <ul class="dropdown-menu" id="nfcdevice_dropdown_menu" role="menu">
                                        </ul>
                                    </div>
                                </div>

                                <div class="col-md-3">
                                    <div class="btn-group">進入管制器:</div>
                                    <div class="btn-group" id="nfclocator_selection"></div>
                                </div>

                                <div class="col-md-3">
                                    <div class="btn-group">人員數:</div>
                                    <h1 class="box-title" id="infield_count">0</h1>
                                </div>
                            </div>
                            <div class="box-body">
                                <table class="table table-bordered">
                                    <thead>
                                        <tr>
                                            <th style="width: 10px;" class="bg-gray disabled">#</th>
                                            <th class="bg-gray disabled">人員</th>
                                            <th class="bg-gray disabled">心跳</th>
                                            <th class="bg-gray disabled">血氧</th>
                                            <th class="bg-gray disabled">氣瓶剩餘量(時間)</th>
                                            <th class="bg-gray disabled">停留時間</th>
                                            <th class="bg-gray disabled">姿態</th>
                                            <th class="bg-gray disabled">狀態</th>
                                        </tr>
                                    </thead>
                                    <tbody id="FIRE_AREA_TABLE_BODY">
                                    </tbody>
                                </table>
                            </div>
                            <div class="box-footer">
                                <div class="col-md-12">
                                    <div id="field_users"></div>
                                </div>
                            </div>
                        </div>

                    </div>
                    <!-- /.col-md-6 -->

                    <div class="col-md-6">

                        <div class="box box-info">
                            <div class="box-header with-border">

                                <div class="col-md-2">
                                    <h3 class="box-title">待命區</h3>
                                </div>

                                <div class="col-md-4">
                                    <div class="btn-group">
                                        <button type="button" class="btn btn-default">選擇待命區定位器</button>
                                        <button type="button" class="btn btn-default dropdown-toggle" data-toggle="dropdown">
                                            <span class="caret"></span>
                                            <span class="sr-only">Toggle Dropdown</span>
                                        </button>
                                        <ul class="dropdown-menu" id="standbydevice_dropdown_menu" role="menu">
                                        </ul>
                                    </div>
                                    <!-- <div class="btn-group">
                                        <label for="aoalocator_range">定位器待命區判定距離(公分):</label>
                                        <input type="text" name="aoalocator_range" id="aoalocator_range" value="300">
                                    </div> -->
                                </div>

                                <div class="col-md-6">
                                    <div class="btn-group">待命區定位器:</div>
                                    <div class="btn-group" id="standbydevice_selection"></div>
                                </div>

                            </div>
                            <div class="box-body">

                                <div class="nav-tabs-custom">
                                    <ul class="nav nav-tabs">
                                        <li class='active'><a href="#tab_standby1" data-toggle='tab' ><span class='badge bg-blue-active'>待命熱區</span><span class='badge bg-yellow' id='standby1_count'>0</span></a></li>
                                        <li><a href="#tab_standby2" data-toggle='tab' ><span class='badge bg-blue'>一般待命區</span><span class='badge bg-yellow' id='standby2_count'>0</span></a></li>
                                        <li><a href="#tab_standby3" data-toggle='tab' ><span class='badge bg-green'>其他人員</span><span class='badge bg-yellow' id='standby3_count'>0</span></a></li>
                                    </ul>
                                    <div class="tab-content">
                                        <div class='tab-pane active table-responsive' id='tab_standby1'>
                                            <table class="table table-bordered">
                                                <thead>
                                                    <tr>
                                                        <th style="width: 10px;" class="bg-gray disabled">#</th>
                                                        <th class="bg-gray disabled">人員</th>
                                                        <th class="bg-gray disabled">心跳</th>
                                                        <th class="bg-gray disabled">血氧</th>
                                                        <th class="bg-gray disabled">氣瓶壓力(單位:bar)</th>
                                                        <th class="bg-gray disabled">姿態</th>
                                                    </tr>
                                                </thead>
                                                <tbody id="STANDBY_AREA_TABLE_BODY">
                                                </tbody>
                                            </table>
                                        </div>

                                        <div class='tab-pane        table-responsive' id='tab_standby2'>
                                            <table class="table table-bordered">
                                                <thead>
                                                    <tr>
                                                        <th style="width: 10px;" class="bg-gray disabled">#</th>
                                                        <th class="bg-gray disabled">人員</th>
                                                        <th class="bg-gray disabled">心跳</th>
                                                        <th class="bg-gray disabled">血氧</th>
                                                        <th class="bg-gray disabled">氣瓶壓力(單位:bar)</th>
                                                        <th class="bg-gray disabled">姿態</th>
                                                    </tr>
                                                </thead>
                                                <tbody id="STANDBY2_AREA_TABLE_BODY">
                                                </tbody>
                                            </table>
                                        </div>

                                        <div class='tab-pane        table-responsive' id='tab_standby3'>
                                            <table class="table table-bordered">
                                                <thead>
                                                    <tr>
                                                        <th style="width: 10px;" class="bg-gray disabled">#</th>
                                                        <th class="bg-gray disabled">人員</th>
                                                        <th class="bg-gray disabled">心跳</th>
                                                        <th class="bg-gray disabled">血氧</th>
                                                        <th class="bg-gray disabled">氣瓶壓力(單位:bar)</th>
                                                        <th class="bg-gray disabled">姿態</th>
                                                    </tr>
                                                </thead>
                                                <tbody id="STANDBY3_AREA_TABLE_BODY">
                                                </tbody>
                                            </table>
                                        </div>
                                    </div>
                                </div>



                            </div>
                            <div class="box-footer">
                            </div>
                        </div>

                    </div>
                    <!-- /.col-md-6 -->

                </div>
                <!-- /.row -->

            </div>
        </div>
    </div>
    <!-- /.col-md-12 -->
</div>
<!-- /.row -->


    <script>
      var project_id = <?php echo $project_id ?>;
      var gl_accountid = "<?php echo $_SESSION["accountid"] ?>";
      var gl_accountname = "<?php echo $_SESSION["accountname"] ?>";
      var gl_groupid = "<?php echo $_SESSION["groupid"] ?>";
      var gl_maingroups = "<?php echo $_SESSION['maingroups'] ?>";
    </script>

    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/leaflet.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/leaflet_draw.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/firefightingrecord.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/firefightingrecord_src/00-Leaflet.ImageOverlay.Rotated.js"></script>
    <script src="/js/firefightingrecord_src/01-Indoor2DMap.js"></script>
    <script src="/js/firefightingrecord_src/02-record.js"></script> -->

</html>
