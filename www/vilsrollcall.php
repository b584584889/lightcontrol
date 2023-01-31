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
    <title>SMIMS VILS IoT - 電子點名</title>
  </head>
      <div class="col-md-12">
                    <div class="box box-default color-palette-box">
                        <div class="box-header with-border">
                            <h3 class="box-title">電子點名</h3>
                        </div>
                        <!-- /.box-header -->

                        <div class="box-body">
                          <div class="row">
                              <div class="col-sm-4 col-md-2">
                                <div class="color-palette-set">
                                  <div class="bg-light-blue disabled color-palette" style="text-align:center;">
                                      <span style="font-size:18px; font-weight:bold; color:#e6f5ff;">應到</span>
                                  </div>
                                  <div class="bg-light-blue color-palette" style="text-align:center;">
                                      <span style="font-size:18px; font-weight:bold; color:#e6f5ff;" id="rollcall_sb">Loading...</span>
                                  </div>
                                  <div style="text-align:center;">
                                      <span style="font-size:18px; font-weight:bold; color:#e6f5ff;">-</span>
                                  </div>
                                </div>
                              </div>
                              <!-- /.col -->
                              <div class="col-sm-4 col-md-2">
                                <div class="color-palette-set">
                                  <div class="bg-green disabled color-palette" style="text-align:center;">
                                      <span style="font-size:18px; font-weight:bold; color:#e6f5ff;">實到</span>
                                  </div>
                                  <div class="bg-green color-palette" style="text-align:center;">
                                      <span style="font-size:18px; font-weight:bold; color:#e6f5ff;" id="rollcall_rb">Loading...</span>
                                  </div>
                                  <div style="text-align:center;">
                                      <span style="font-size:18px; font-weight:bold; color:#e6f5ff;">-</span>
                                  </div>
                                </div>
                              </div>
                              <!-- /.col -->
                              <div class="col-sm-4 col-md-2">
                                <div class="color-palette-set">
                                  <div class="bg-red disabled color-palette" style="text-align:center;">
                                      <span style="font-size:18px; font-weight:bold; color:#e6f5ff;">未到</span>
                                  </div>
                                  <div class="bg-red color-palette" style="text-align:center;">
                                      <span style="font-size:18px; font-weight:bold; color:#e6f5ff;" id="rollcall_nb">Loading...</span>
                                  </div>
                                  <div style="text-align:center;">
                                      <span style="font-size:18px; font-weight:bold; color:#e6f5ff;">-</span>
                                  </div>
                                </div>
                              </div>
                              <!-- /.col -->
                              <!-- /.col -->
                              <div class="col-sm-4 col-md-2">
                                <div class="color-palette-set">
                                  <div class="bg-black disabled color-palette" style="text-align:center;">
                                      <span style="font-size:18px; font-weight:bold; color:#e6f5ff;">點名時間</span>
                                  </div>
                                  <div class="bg-black color-palette" style="text-align:center;">
                                      <span style="font-size:18px; font-weight:bold; color:#e6f5ff;" id="rollcall_time">Loading...</span>
                                  </div>
                                  <div style="text-align:center;">
                                      <span style="font-size:18px; font-weight:bold; color:#e6f5ff;">-</span>
                                  </div>
                                </div>
                              </div>
                              <!-- /.col -->

                              <!-- /.col -->
                              <div class="col-sm-4 col-md-2">
                                  <a class="btn btn-success btn-xs" onclick="create_tag_pdf_report();" id="pdfreport_btn">產生PDF報告</a>
                                  <span id="pdfreport_status"></span>
                                  <a class="btn btn-success btn-xs" onclick="create_tag_xlsx_report();" id="xlsxreport_btn">產生XLSX報告</a>
                                  <span id="xlsxreport_status"></span>
                              </div>
                              <!-- /.col -->

                          </div>
                          <!-- /.row -->

                          <div class="row">
                            <div class="col-sm-12 col-md-12">
                                <table class="table table-bordered" id="NODE_TABLE">
                                    <thead>
                                        <tr>
                                            <th style="width: 80px;" class="bg-gray disabled">狀態</th>
                                            <th style="width: 50px;" class="bg-gray disabled">人數</th>
                                            <th class="bg-gray disabled">人員名單</th>
                                        </tr>
                                    </thead>
                                    <tbody id="NODE_TABLE_BODY">
                                    </tbody>
                                </table>
                            </div>
                            <!-- /.col -->
                          </div>
                          <!-- /.row -->

                        </div>
                        <!-- /.box-body -->
                    </div>
                    <!-- /.box -->

            </div>
            <!-- /.col-md-12 -->
    <script>
      var project_id = <?php echo $project_id ?>;
      var accountid = "<?php echo $_SESSION["accountid"] ?>";
      var accountname = "<?php echo $_SESSION["accountname"] ?>";
      var groupid = "<?php echo $_SESSION["groupid"] ?>";
      var rollcall_areaid = "<?php echo $areaid ?>";
    </script>

    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/rollcall.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/rollcall_src/00-rollcall.js"></script> -->

</html>
