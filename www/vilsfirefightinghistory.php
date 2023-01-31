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
    <title><?php echo $firefighting_history; ?></title>
  </head>

<div class="row">
    <div class="col-md-12">
        <div class="box box-primary">
            <div class="box-header with-border">
                <h3 class="box-title"><?php echo $firefighting_history; ?></h3>
                <div class="box-tools pull-right">
                    <button type="button" class="btn btn-box-tool" onclick="open_firefighting_history_page();return false;"><i class="fa fa-external-link"></i>
                    </button>
                </div>
            </div>

            <div class="nav-tabs-custom">
                <ul class="nav nav-tabs">
                    <li class='active'><a href="#tab_manualinput" data-toggle='tab' ><span class='badge bg-blue-active'>手動輸入案號</span></a></li>
                    <li style='display:none;'><a href="#tab_dateinput" data-toggle='tab' ><span class='badge bg-blue'>使用日期選擇案號</span></a></li>
                    <li style='display:none;'><a href="#tab_mapinput" data-toggle='tab' ><span class='badge bg-green'>使用選擇案號</span></a></li>
                </ul>
                <div class="tab-content">
                    <div class='tab-pane active table-responsive' id='tab_manualinput'>

                        <input type="text" id="manual_search_text" onfocus="manual_search_text_onfocus(this.id)" placeholder="輸入案號">
                        <ul class="dropdown-menu" id="dropdown_searchresult">
                        </ul>

                    </div>

                    <div class='tab-pane        table-responsive' id='tab_dateinput'>

                    </div>

                    <div class='tab-pane        table-responsive' id='tab_mapinput'>
                        <div id="mapid" style="height:200px;"> </div>
                    </div>
                </div>
            </div>



        </div>
    </div>
    <!-- /.col-md-12 -->
</div>
<!-- /.row -->

<div class="row">
    <div class="col-md-12">
        <div class="box box-default">
            <div class="box-header">
            </div>
        </div>

        <div class="box-body">
            <div class="row">
                <div class="col-md-4">

                    <table class="table table-bordered">
                        <thead>
                        </thead>
                        <tbody style="background-color:rgb(230, 230, 230);">
                            <tr>
                                <td><div>火場名稱:</div></td>
                                <td><div><span id="record_address"></span></div></td>
                            </tr>
                            <tr>
                                <td><div>火場記錄開始時間:</div></td>
                                <td><div><span id="record_starttime"></span></div></td>
                            </tr>
                            <tr>
                                <td><div>火場記錄結束時間:</div></td>
                                <td><div><span id="record_closetime"></span></div></td>
                            </tr>
                            <tr>
                                <td><div>待命區定位器:</div></td>
                                <td><div><span id="record_StandbyDeviceIds"></span></div></td>
                            </tr>
                            <tr>
                                <td><div>進入管制器:</div></td>
                                <td><div><span id="record_NFCDeviceIds"></span></div></td>
                            </tr>
                            <tr>
                                <td><div>參與人員:</div></td>
                                <td><div id="record_Users"></div></td>
                            </tr>
                        </tbody>
                    </table>

                </div>
                <!-- /.col-md-4 -->

                <div class="col-md-8">

                    <table class="table table-bordered">
                        <thead style="background-color:rgb(230, 230, 230);">
                            <tr>
                                <th>時間</th>
                                <th>事件型態</th>
                                <th>事件內容</th>
                            </tr>
                        </thead>
                        <tbody id="record_events" style="background-color:rgb(210, 230, 230);">

                        </tbody>
                    </table>

                </div>
                <!-- /.col-md-8 -->

            </div>
            <!-- /.row -->
        </div>
        <!-- /.box-body -->
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
    <script src="/js/firefightinghistory.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/firefightinghistory_src/00-Leaflet.ImageOverlay.Rotated.js"></script>
    <script src="/js/firefightinghistory_src/01-Indoor2DMap.js"></script>
    <script src="/js/firefightinghistory_src/02-history.js"></script> -->

</html>
