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
    <title>SMIMS VILS IoT - Scott Test</title>
    <style>
        thead tr {
            background-color:rgb(236, 240, 245);
        }
    </style>
  </head>

  <div class="box">

    <div class="box-header with-border">
      <h3 class="box-title"><?php echo $vilsevnetreport_title;?></h3>
      <div class="box-tools pull-right">
        <ul class="pagination pagination-sm no-margin pull-right">
          <li>
            <button type="button" class="btn btn-box-tool" onclick="open_eventreport_page();return false;">
              <i class="fa fa-external-link"></i>
            </button>
          </li>
        </ul>
      </div>
    </div>
    <!-- /.box-header -->
    <div class="box-body">
        <div class="nav-tabs-custom" >
            <ul class="nav nav-tabs">
                <?php
                    echo "<li class='active'><a href=\"#tab_other\" data-toggle='tab' onclick=\"scott_tab_onclick('other');\"><span class='badge bg-aqua-active'>"; echo $vilsscott_other; echo "</span></a></li>";
                    echo "<li><a href=\"#tab_light\" data-toggle='tab' onclick=\"scott_tab_onclick('light');\"><span class='badge bg-yellow-active'>"; echo $vilsscott_light; echo "</span></a></li>";
                    echo "<li><a href=\"#tab_uw\" data-toggle='tab' onclick=\"scott_tab_onclick('uw');\"><span class='badge bg-red-active'>"; echo $vilsscott_uw; echo "</span></a></li>";
                    echo "<li><a href=\"#tab_watch\" data-toggle='tab' onclick=\"scott_tab_onclick('watch');\"><span class='badge bg-green-active'>"; echo $vilsscott_watch; echo "</span></a></li>";
                    echo "<li><a href=\"#tab_pm\" data-toggle='tab' onclick=\"scott_tab_onclick('pm');\"><span class='badge bg-blue-active'>"; echo $vilsscott_pm; echo "</span></a></li>";
                ?>
            </ul>
            <div class="tab-content">
                <div class='tab-pane active table-responseive' id='tab_other'>
                    <a class="btn btn-success btn-xs" onclick="return false;" id="xlsxreport_btn">產生XLSX檔案</a>
                    <span id="xlsxreport_status"></span>

                    <div class="box-tools pull-right">
                      <ul class="pagination pagination-sm no-margin">
                        <li><a href="#" onclick="move_page_backward('other');return false;">&laquo;</a></li>
                        <li><a href="#" id="id_other_current_page_num">1</a></li>
                        <li><a href="#" onclick="move_page_forward('other');return false;">&raquo;</a></li>
                      </ul>
                    </div>

                    <table class="table table-bordered">
                      <thead>
                        <tr>
                          <th style="width:10px">#</th>
                          <th style="width:50px" data-field="projectid">專案代碼</th>
                          <th style="width:50px" data-field="eventid">事件編號</th>
                          <th style="width:50px" data-field="nodeid">節點編號</th>
                          <th style="width:50px" data-field="action">動作</th>
                          <th style="width:50px" data-field="issueTime">發生時間</th>
                        </tr>
                      </thead>
                      <tbody id="table_other">
                      </tbody>
                    </table>
                    <div class='row'>
                        <div id='div_other_chart_area'>
                        </div>
                    </div>
                </div>
                <div class='tab-pane table-responseive' id='tab_light'>
                    <a class="btn btn-success btn-xs" onclick="return false;" id="xlsxreport_btn">產生XLSX檔案</a>
                    <span id="xlsxreport_status"></span>

                    <div class="box-tools pull-right">
                      <ul class="pagination pagination-sm no-margin">
                        <li><a href="#" onclick="move_page_backward('light');return false;">&laquo;</a></li>
                        <li><a href="#" id="id_light_current_page_num">1</a></li>
                        <li><a href="#" onclick="move_page_forward('light');return false;">&raquo;</a></li>
                      </ul>
                    </div>
                    <table class='table table-bordered'>
                        <thead>
                            <tr>
                                <th data-field='id'>ID</th>
                                <th data-field='name'>名稱</th>
                                <th data-field='uniAddress'>uniAddress</th>
                                <th data-field='state_onoff'>開關</th>
                                <th data-field='state_level'>亮度</th>
                                <th data-field='state_cct'>色溫</th>
                                <th data-field='datetime'>發生時間</th>
                            </tr>
                        </thead>
                        <tbody id='table_light'>
                        </tbody>
                    </table>
                    <div class='row'>
                        <div id='div_light_chart_area'></div>
                    </div>
                </div>
                <div class='tab-pane table-responseive' id='tab_uw'>
                    <a class="btn btn-success btn-xs" onclick="return false;" id="xlsxreport_btn">產生XLSX檔案</a>
                    <span id="xlsxreport_status"></span>

                    <div class="box-tools pull-right">
                      <ul class="pagination pagination-sm no-margin">
                        <li><a href="#" onclick="move_page_backward('uw');return false;">&laquo;</a></li>
                        <li><a href="#" id="id_uw_current_page_num">1</a></li>
                        <li><a href="#" onclick="move_page_forward('uw');return false;">&raquo;</a></li>
                      </ul>
                    </div>
                    <table class='table table-bordered'>
                        <thead>
                            <tr>
                                <th data-field='Device_ID'>ID</th>
                                <th data-field='Rssi'>RSSI</th>
                                <th data-field='datetime'>發生時間</th>
                            </tr>
                        </thead>
                        <tbody id='table_uw'>
                        </tbody>
                    </table>
                    <div class='row'>
                        <div id='div_uw_chart_area'></div>
                    </div>
                </div>
                <div class='tab-pane table-responseive' id='tab_watch'>
                    <a class="btn btn-success btn-xs" onclick="return false;" id="xlsxreport_btn">產生XLSX檔案</a>
                    <span id="xlsxreport_status"></span>

                    <div class="box-tools pull-right">
                      <ul class="pagination pagination-sm no-margin">
                        <li><a href="#" onclick="move_page_backward('watch');return false;">&laquo;</a></li>
                        <li><a href="#" id="id_watch_current_page_num">1</a></li>
                        <li><a href="#" onclick="move_page_forward('watch');return false;">&raquo;</a></li>
                      </ul>
                    </div>
                    <table class='table table-bordered'>
                        <thead>
                            <tr>
                                <th data-field='Device_ID'>ID</th>
                                <th data-field='Blood_Oxygen'>血氧</th>
                                <th data-field='DBP'>舒張壓</th>
                                <th data-field='SBP'>收縮壓</th>
                                <th data-field='Heart_Rate'>脈搏</th>
                                <th data-field='Rssi'>RSSI</th>
                                <th data-field='SOS'>緊急模式</th>
                                <th data-field='Temperature'>體溫</th>
                                <th data-field='datetime'>發生時間</th>
                            </tr>
                        </thead>
                        <tbody id='table_watch'>
                        </tbody>
                    </table>
                    <div class='row'>
                        <div id='div_watch_chart_area'></div>
                    </div>
                </div>
                <div class='tab-pane table-responseive' id='tab_pm'>
                    <a class="btn btn-success btn-xs" onclick="return false;" id="xlsxreport_btn">產生XLSX檔案</a>
                    <span id="xlsxreport_status"></span>

                    <div class="box-tools pull-right">
                      <ul class="pagination pagination-sm no-margin">
                        <li><a href="#" onclick="move_page_backward('pm');return false;">&laquo;</a></li>
                        <li><a href="#" id="id_pm_current_page_num">1</a></li>
                        <li><a href="#" onclick="move_page_forward('pm');return false;">&raquo;</a></li>
                      </ul>
                    </div>
                    <table class='table table-bordered'>
                        <thead>
                            <tr>
                                <th data-field='Device_ID'>ID</th>
                                <th data-field='Active_Energy'>功耗</th>
                                <th data-field='Active-Power'>功率</th>
                                <th data-field='BT_version'>版本</th>
                                <th data-field='Current_RMS'>電流</th>
                                <th data-field='Frequency'>頻率</th>
                                <th data-field='Rssi'>RSSI</th>
                                <th data-field='Voltage_RMS'>電壓</th>
                                <th data-field='relay_state'>Relay狀態</th>
                                <th data-field='datetime'>發生時間</th>
                            </tr>
                        </thead>
                        <tbody id='table_pm'>
                        </tbody>
                    </table>
                    <div class='row'>
                        <!-- <div id='div_pm_chart_area'></div> -->
                    </div>
                </div>
            </div>
        </div>
    </div>

    <!-- /.box-body -->

  </div>

  <div class="box" style="display:none;">
      <div class="box-header with-border">
        <h3 class="box-title"><?php echo $vilsevnetreport_title2;?></h3>
        <div class="box-tools pull-right">
          <ul class="pagination pagination-sm no-margin pull-right">
            <li>
              <button type="button" class="btn btn-box-tool" onclick="open_eventreport_page();return false;">
                <i class="fa fa-external-link"></i>
              </button>
            </li>
          </ul>
        </div>
      </div>
      <div class="box-body">
          <div class="">
              <div class="row">
                  <div class="col-xs-4 col-sm-4 col-md-4 col-lg-4">
                    <div class="form-group">
                          <label>選擇查詢區間:</label>

                          <div class="input-group">
                            <button type="button" class="btn btn-default pull-right" id="daterange-btn">
                              <span>
                                <i class="fa fa-calendar"></i> 查詢區間
                              </span>
                              <i class="fa fa-caret-down"></i>
                            </button>
                          </div>
                        </div>
                  </div>

              </div>
              <div class="row">
                  <div id='div_pm_chart_area'></div>
              </div>
          </div>
      </div>
  </div>
  <!-- /.box -->

  <script>
    var project_id = <?php echo $project_id ?>;
    var gl_accountid = "<?php echo $_SESSION["accountid"] ?>";
    var gl_accountname = "<?php echo $_SESSION["accountname"] ?>";
    var gl_groupid = "<?php echo $_SESSION["groupid"] ?>";
  </script>

  <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
  <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
  <script src="/js/eventreport_src/00-raphael.js?s=1"></script>
  <script src="/js/eventreport_src/01-morris.js?s=1"></script>
  <script src="/js/eventreport_src/03-checkeventfired.js?s=1"></script>
  <script src="/js/eventreport_src/04-jquery.knob.js?s=1"></script>
  <script src="/js/eventreport_src/05-daterangepicker.js?s=1"></script>
  <script src="/js/eventreport_src/02-eventreport.js?s=2"></script>

</html>
