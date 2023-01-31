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
    <link href="/css/breadcrumb.css" rel="stylesheet">
    <meta name="description" content="SMIMS VILS IoT">
    <meta name="author" content="SMIMS">
    <title>SMIMS VILS IoT - Diagnosis</title>
  </head>

        <div class="row table-responsive" style="height:100vh;">
          <div class="col-md-12">

            <!-- MAP & BOX PANE -->
            <div class="box box-success">
              <div class="box-header with-border">
                <h3 class="box-title">SMIMS VILS Diagnosis</h3>
                <div class="box-tools pull-right">
                  <button type="button" class="btn btn-box-tool" onclick="open_diagnosis_page();return false;"><i class="fa fa-external-link"></i>
                  </button>
                </div>
              </div>
              <!-- /.box-header -->

              <div class="box-body no-padding" style="height:100vh;">

                  <!-- <div class="row">
                      <div class="col-md-12 col-sm-12">
                          <div class="pad">
                              <div class="btn-group">
                                  <button type="button" class="btn btn-default btn-sm dropdown-toggle" data-toggle="dropdown" id="dropdown_cur_tag">Show Tag<span class="caret"></span></button>
                                  <ul class="dropdown-menu" id="dropdown_tags">
                                  </ul>
                              </div>
                          </div>
                      </div>
                  </div> -->

                  <div class="row" style="height:100vh;">
                      <div class="col-md-12 text-center">
                          <div id="DIAGNOSIS_TABLE_MSG">Loading...</div>
                          <!-- Custom Tabs (Pulled to the right) -->

                        <div class="box table-responsive">
                          <div class="box-header">
                            <!-- <h3 class="box-title">Realtime Diagnosis</h3> -->
                          </div>
                          <!-- /.box-header -->

                          <!-- Custom Tabs (Pulled to the right) -->
                          <div class="nav-tabs-custom">
                            <ul class="nav nav-tabs">
                              <li class="active"><a href="#loc_tab_tags" data-toggle="tab"><span class="badge bg-blue-active">Tags</span></a></li>
                              <li><a href="#loc_tab_locators" data-toggle="tab"><span class="badge bg-yellow-active">Locators</span></a></li>
                              <!-- <li><a href="#loc_tab_coords" data-toggle="tab"><span class="badge bg-red-active">Coordinators</span></a></li> -->
                            </ul>
                            <div class="tab-content" style="height:100vh;">

                              <div class="tab-pane active" id="loc_tab_tags">

                                  <div class="btn-group pull-left">
                                      <button type="button" class="btn btn-default btn-sm dropdown-toggle" data-toggle="dropdown" id="dropdown_cur_tag">Show Tag<span class="caret"></span></button>
                                      <ul class="dropdown-menu" id="dropdown_tags">
                                      </ul>
                                  </div>

                                  <table class="table table-bordered">
                                      <thead>
                                      <tr style="background-color:#cce6ff;">
                                          <th style="width: 10px">#</th>
                                          <th style="text-align:center;">Anchor</th>
                                          <th style="text-align:center;">Ranging</th>
                                          <th style="text-align:center;">SD</th>
                                          <th style="text-align:center;">Lost</th>
                                          <th style="text-align:center;">DataRate/ValidRate</th>
                                          <th style="text-align:center;">DataCount</th>
                                          <th style="text-align:center;">SlotTime/CycleTime/DataRate</th>
                                          <th style="text-align:center;">R/FP/AnR/AnFP</th>
                                          <th style="text-align:center;">V</th>
                                          <th style="text-align:center;">AnCnt</th>
                                          <th style="text-align:center;">IMU</th>
                                      </tr>
                                      </thead>
                                      <tbody id="DIAGNOSIS_TABLE_CONTENT">
                                      </tbody>
                                  </table>
                              </div>
                              <!-- /.tab-pane -->

                              <div class="tab-pane table-responsive" id="loc_tab_locators" style="height:100vh;">

                                    <div class="btn-group pull-left">
                                        <button type="button" class="btn btn-default btn-sm dropdown-toggle" data-toggle="dropdown" id="dropdown_cur_locator">Show Locator<span class="caret"></span></button>
                                        <ul class="dropdown-menu" id="dropdown_locators">
                                        </ul>
                                    </div>

                                  <table class="table table-bordered">
                                      <thead>
                                      <tr style="background-color:#cce6ff;">
                                          <th style="width: 10px">#</th>
                                          <th style="text-align:center;">Gateway</th>
                                          <th style="text-align:center;">RSSI</th>
                                          <th style="text-align:center;">Count</th>
                                          <th style="text-align:center;">Start time</th>
                                          <th style="text-align:center;">End time</th>
                                      </tr>
                                      </thead>
                                      <tbody id="LOCATOR_TABLE_CONTENT">
                                      </tbody>
                                  </table>
                              </div>
                              <!-- /.tab-pane -->

                            </div>
                            <!-- /.tab-content -->
                          </div>
                          <!-- nav-tabs-custom -->

                              <!-- /.box-body -->
                            </div>
                            <!-- /.box -->
                      </div>
                      <!-- ./col -->
                  </div>
                  <!-- /.row -->

              </div>
              <!-- /.box-body -->
            </div>
            <!-- /.box box-success -->
            <!-- /.MAP & BOX PANE -->
          </div>
          <!-- /.col-md-12 -->
        </div>
        <!-- /.row -->


    <script>
      var project_id = <?php echo $project_id ?>;
      var accountid = "<?php echo $_SESSION["accountid"] ?>";
      var accountname = "<?php echo $_SESSION["accountname"] ?>";
      var groupid = "<?php echo $_SESSION["groupid"] ?>";
    </script>

    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/diagnosis.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/diagnosis_src/00-diagnosis.js"></script> -->

</html>
