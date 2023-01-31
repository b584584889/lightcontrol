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
    <title>SMIMS VILS IoT - <?php echo $left_side_bar_bodyinfo_manual_input; ?></title>
  </head>

<div class="wrapper">
<div class="content-wrapper" style="margin:0; padding:0; border:0 none;">

<section class="content" style="margin:0; padding:0; border:0 none;">

 <div class="row">

  <div class="col-md-12 col-sd-12">

      <!-- weekschedule -->
      <div class="box">
        <div class="box-header with-border">
          <h3 class="box-title"><?php echo $left_side_bar_bodyinfo_manual_input; ?></h3>

          <span>&nbsp;&nbsp;</span>
          <div class="btn-group">
            <button type="button" class="btn btn-default">NFC感應器(刷卡調閱人員)</button>
            <button type="button" class="btn btn-default dropdown-toggle" data-toggle="dropdown">
              <span class="caret"></span>
              <span class="sr-only">Toggle Dropdown</span>
            </button>
            <ul class="dropdown-menu" id="nfcdevice_dropdown_menu" role="menu">
            </ul>
            <span>&nbsp;&nbsp;</span>
            <label id="cardscan_result_label"></label>
            </div>

          <div class="box-tools pull-right">
              <button type="button" class="btn btn-box-tool" onclick="open_bodyinfomanualinput_page();return false;"><i class="fa fa-external-link"></i>
              </button>
          </div>
        </div>
        <!-- /.box-header -->

        <div class="box-body table-responsive">

            <div class="row">
              <div class="col-md-12 col-sd-12">

                    <table class="table table-bordered" style="border-color:black;border:3px #111111 solid;">
                        <thead>
                          <tr style="background-color:#acdafb;border-color:black;">
                            <th style="text-align:center;border-color:black;" colspan="8">生理量測紀錄</th>
                          </tr>
                        </thead>
                        <tbody style="text-align:center;">
                          <tr>
                            <td style="background-color:#cce6ff;border-color:black;font-weight:bold;" colspan="2">人員量測紀錄</td>
                            <td style="background-color:#cce6ff;border-color:black;font-weight:bold;" colspan="2">紀錄人員</td>
                            <td style="background-color:#cce6ff;border-color:black;font-weight:bold;" colspan="4">紀錄時間</td>
                          </tr>
                          <tr>
                            <td style="border-color:black;" colspan="2" id="target_user"></td>
                            <td style="border-color:black;" colspan="2" id="recorder_user"></td>
                            <td style="border-color:black;background-color:#eae6ed;" colspan="4"><input type="text" id="bodyinfo_datetime"></td>
                          </tr>

                          <tr>
                              <td style="border-color:black;" colspan="8"> </td>
                          </tr>

                          <tr>
                            <td style="background-color:#c0e7cd;border-color:black;">體重</td>
                            <td style="background-color:#c0e7cd;border-color:black;">心率</td>
                            <td style="background-color:#c0e7cd;border-color:black;">血糖</td>
                            <td style="background-color:#c0e7cd;border-color:black;">收縮壓</td>
                            <td style="background-color:#c0e7cd;border-color:black;">舒張壓</td>
                            <td style="background-color:#c0e7cd;border-color:black;">體溫</td>
                            <td style="background-color:#c0e7cd;border-color:black;">體脂率</td>
                            <td style="background-color:#c0e7cd;border-color:black;">血氧</td>
                          </tr>

                            <tr style="background-color:#eae6ed;">
                              <td style="border-color:black;vertical-align:middle;"><input type="text" id="bodyinfo_weight" style="width:80px;"></td>
                              <td style="border-color:black;vertical-align:middle;"><input type="text" id="bodyinfo_heartbeat" style="width:80px;"></td>
                              <td style="border-color:black;vertical-align:middle;"><input type="text" id="bodyinfo_bloodsugar" style="width:80px;"></td>
                              <td style="border-color:black;vertical-align:middle;"><input type="text" id="bodyinfo_systolicbloodpressure" style="width:80px;"></td>
                              <td style="border-color:black;vertical-align:middle;"><input type="text" id="bodyinfo_diastolicbloodpressure" style="width:80px;"></td>
                              <td style="border-color:black;vertical-align:middle;"><input type="text" id="bodyinfo_bodytemperature" style="width:80px;"></td>
                              <td style="border-color:black;vertical-align:middle;"><input type="text" id="bodyinfo_bodyfat" style="width:80px;"></td>
                              <td style="border-color:black;vertical-align:middle;"><input type="text" id="bodyinfo_bloodoxygen" style="width:80px;"></td>
                            </tr>

                            <tr>
                                <td style="border-color:black;" colspan="8">
                                    <div class="btn-group">
                                    <button class='btn btn-block btn-warning btn-lg' id='savebodyinfobutton'><i class='fa fa-plus'></i> 儲存 </button>
                                </div>
                                </td>
                            </tr>

                        </tbody>
                        <tfoot>
                          <tr>
                            <td style="border-color:black;" colspan="8"></td>
                          </tr>
                        </tfoot>
                    </table>

                </div>
                <!-- /.col -->
            </div>
            <!-- /.row -->

            <div class="row" id="user_body_info_table" style="display:none;">
              <div class="col-md-12 col-sd-12">
                    <table class="table table-bordered" style="border-color:black;border:3px #111111 solid;">
                        <thead>
                          <tr style="background-color:#acdafb;border-color:black;">
                            <th style="text-align:center;border-color:black;" colspan="8" id="user_body_info_title">生理量測紀錄</th>
                          </tr>
                        </thead>
                        <tbody style="text-align:center;">
                          <tr>
                            <td style="background-color:#cce6ff;border-color:black;font-weight:bold;" colspan="2">
                              <div id='show_chart_area'>
                            </td>
                          </tr>
                        </tbody>
                        <tfoot>
                          <tr>
                            <td style="border-color:black;" colspan="8"></td>
                          </tr>
                        </tfoot>
                    </table>

                </div>
                <!-- /.col -->
            </div>
            <!-- /.row -->

        </div>
        <!-- /.box-body-->

      </div>
      <!-- /.box-->

    </div>
    <!-- /.col -->

  </div>
  <!-- /.row -->


</section>
</div>
<!-- content-wrapper -->
</div>
<!-- ./wrapper -->


    <script>
        var project_id = <?php echo $project_id ?>;
        var gl_accountid = "<?php echo $_SESSION["accountid"] ?>";
        var gl_accountname = "<?php echo $_SESSION["accountname"] ?>";
        var gl_groupid = "<?php echo $_SESSION["groupid"] ?>";
    </script>


    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/bodyinfomanualinput.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/bodyinfomanualinput_src/00-raphael.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/bodyinfomanualinput_src/01-morris.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/bodyinfomanualinput_src/02-jquery.knob.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/bodyinfomanualinput_src/03-daterangepicker.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/bodyinfomanualinput_src/04-bodyinfomanualinput.js?date=<?php echo $UPDATE_DATE;?>"></script> -->

</html>
