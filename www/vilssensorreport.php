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

    <style>
            .top {
                border-collapse: collapse;
                border-top: 1px black solid;
            }

            .tabletr {
                line-height: 20px;
            }

            .tabletd {
                font-weight: normal;
                border: black 1px solid;
                text-align: center;
            }


    </style>

    <title>SMIMS VILS IoT - <?php echo $vilssensor_report_title;?></title>
  </head>

    <div class="col-md-12 col-sd-12">
      <!-- Event Settings -->
      <div class="box">
        <div class="box-header with-border">
          <h3 class="box-title"><?php echo $vilssensor_report_title;?></h3>
          <div class="box-tools pull-right">
              <button type="button" class="btn btn-box-tool" onclick="open_sensor_report_page();return false;"><i class="fa fa-external-link"></i>
              </button>
              <!-- <a href="javascript:void(0)" onclick="open_sections_page();return false;"><i class="fa fa-external-link"></i></a> -->
          </div>
        </div>
        <!-- /.box-header -->

          <div class="box-body">
              <!-- Custom Tabs (Pulled to the right) -->
              <div class="nav-tabs-custom">

                <ul class="nav nav-tabs">
                    <!-- <li><a href="#tab_daily_report" data-toggle="tab" onclick="tab_daily_onclick();"><span class="badge bg-blue-active"><?php echo $vilssensor_daily_report;?></span></a></li> -->
                    <!-- <li><a href="#tab_weekly_report" data-toggle="tab" onclick="tab_weekly_onclick();"><span class="badge bg-green-active"><?php echo $vilssensor_weekly_report;?></span></a></li> -->
                    <li class="active"><a href="#tab_monthly_report" data-toggle="tab" onclick="tab_monthly_onclick();"><span class="badge bg-red-active"><?php echo $vilssensor_monthly_report;?></span></a></li>
                </ul>
                <div class="tab-content">
                    <!-- <div class="tab-pane active table-responsive" id="tab_daily_report">
                        <ul class="pagination pagination-sm no-margin pull-left">
                            <li><a href="javascript:;" onclick="move_daily_backward();return false;">&laquo</a></li>
                            <li><a href="#" id="daily_targetdate">2020/01/01</a></li>
                            <li><a href="javascript:;" onclick="move_daily_forward();return false;">&raquo</a></li>
                        </ul>

                        <span class='pull-right' id="daily_pdf_report_status"></span>
                        <button type='button' class='btn btn-app pull-right' onclick='get_daily_pdf_report();return false;'><i class='fa fa-file-pdf-o'></i>產生報表</button>

                        <table class="table table-bordered" style="border-color:black;">
                          <thead>
                              <tr id="DAILY_REPORT_TABLE_HEADER">
                              </tr>
                          </thead>
                          <tbody id="DAILY_REPORT_TABLE">
                          </tbody>
                        </table>
                    </div> -->

                    <!-- <div class="tab-pane table-responsive" id="tab_weekly_report">
                        <ul class="pagination pagination-sm no-margin pull-left">
                            <li><a href="javascript:;" onclick="move_weekly_backward();return false;">&laquo</a></li>
                            <li><a href="#" id="weekly_targetdate">2020/01/01</a></li>
                            <li><a href="javascript:;" onclick="move_weekly_forward();return false;">&raquo</a></li>
                        </ul>

                        <span class='pull-right' id="weekly_pdf_report_status"></span>
                        <button type='button' class='btn btn-app pull-right' onclick='get_weekly_pdf_report();return false;'><i class='fa fa-file-pdf-o'></i>產生報表</button>

                        <table class="table table-bordered" style="border-color:black;">
                          <thead>
                            <tr id="WEEKLY_REPORT_TABLE_HEADER">
                            </tr>
                          </thead>
                          <tbody id="WEEKLY_REPORT_TABLE">
                          </tbody>
                        </table>
                    </div> -->

                    <div class="tab-pane table-responsive active" id="tab_monthly_report">

                        <div style="border-color:black;text-align: center; height: 80px;">
                            <ul class="pagination pagination-sm no-margin pull-left">
                                <li><a href="javascript:;" onclick="move_monthly_backward();return false;">&laquo</a></li>
                                <li><a href="#" id="monthly_targetdate">2020/01/01</a></li>
                                <li><a href="javascript:;" onclick="move_monthly_forward();return false;">&raquo</a></li>
                            </ul>

                            <span class='pull-right' id="monthly_pdf_report_status"></span>
                            <button type='button' class='btn btn-app pull-right' onclick='get_monthly_pdf_report();return false;'><i class='fa fa-file-pdf-o'></i>產生報表</button>
                        </div>

                        <!-- <table class="table table-bordered" style="border-color:black;text-align: center; font-size: 20px; padding: 0; height: 2000px; width: 1395px; table-layout: fixed;"id="MONTHLY_REPORT_TABLE_PDF"> -->
                        <table class="table table-bordered" style="border-color:black;text-align: center; width: 1395px;" id="MONTHLY_REPORT_TABLE_PDF">
                          <thead>
                            <tr class="tabletr" id="MONTHLY_REPORT_TABLE_HEADER">
                            </tr>
                          </thead>
                          <tbody class="top" id="MONTHLY_REPORT_TABLE">
                          </tbody>
                        </table>

                    </div>

                </div>
                <!-- /.tab-content -->

              </div>
              <!-- nav-tabs-custom -->
              <div id="INSPECTION_REPORT_TABLE_MSG"><?php echo $vilssensorrecord_loading;?></div>
          </div>
          <!-- /.box-body -->
        </div>
        <!-- /.box-->
      </div>
      <!-- /.col -->

    <script>
        var project_id = <?php echo $project_id ?>;
        var accountid = "<?php echo $_SESSION["accountid"] ?>";
        var accountname = "<?php echo $_SESSION["accountname"] ?>";
        var groupid = "<?php echo $_SESSION["groupid"] ?>";
        var gl_maingroups = "<?php echo $_SESSION['maingroups'] ?>";
    </script>

    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <!-- <script src="/js/sensorreport.min.js?date=<?php echo $UPDATE_DATE;?>"></script> -->

    <script src="/js/sensorreport_src/00-bootstrap-datepicker.js"></script>
    <script src="/js/sensorreport_src/01-sensorreport.js"></script>
    <script src="/js/sensorreport_src/02-pdfmake.min.js"></script>
    <script src="/js/sensorreport_src/03-html2canvas.min.js"></script>

</html>
