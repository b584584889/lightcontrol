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
    <title>SMIMS VILS IoT - Analyzer</title>
  </head>

        <div class="row">
          <div class="col-md-12">

            <!-- MAP & BOX PANE -->
            <div class="box box-success">
              <div class="box-header with-border">
                <h3 class="box-title">Indoor Map</h3>
                <div class="box-tools pull-right">
                  <button type="button" class="btn btn-box-tool" onclick="open_analyzer_page();return false;"><i class="fa fa-external-link"></i>
                  </button>
                  <button type="button" class="btn btn-box-tool"  data-toggle="control-sidebar" onclick="toggleOn();"><i class="fa fa-gears"></i>
                  </button>
                </div>
              </div>
              <!-- /.box-header -->

              <div class="box-body no-padding">
                <div class="row">
                    <div class="col-md-12 col-sm-12">
                        <div class="pad">
                            <div class="btn-group">
                                <button type="button" class="btn btn-default btn-sm dropdown-toggle" data-toggle="dropdown">Show Records<span class="caret"></span></button>
                                <ul class="dropdown-menu" id="dropdown_files">
                                </ul>
                            </div>
                            <div class="btn-group">
                                <button type="button" class="btn btn-default btn-sm dropdown-toggle" data-toggle="dropdown">Show Tags<span class="caret"></span></button>
                                <ul class="dropdown-menu" id="dropdown_tags">
                                </ul>
                            </div>
                        </div>
                    </div>
                    <!-- /.col -->
                </div>
                <!-- /.row -->

                <div class="row">
                  <div class="col-md-12 col-sm-12">
                    <div class="pad">
                      <!-- Map will be created here -->
                      <div id="mapid" style="height:85vh"></div>
                    </div>
                  </div>
                  <!-- /.col -->
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



        <div class="row" style="display:none;">
          <div class="col-md-12">

            <!-- MAP & BOX PANE -->
            <div class="box box-success">
              <div class="box-header with-border">
                <h3 class="box-title">Analysis</h3>
                <div class="box-tools pull-right">
                </div>
              </div>
              <!-- /.box-header -->

              <div class="box-body no-padding">
                  <div class="row">

                  <div class="col-md-4">
                    <div class="box box-solid">
                      <div class="box-header">
                        <h3 class="box-title text-blue">Sparkline line</h3>

                        <div class="box-tools pull-right">
                          <button type="button" class="btn btn-default btn-sm"><i class="fa fa-refresh"></i></button>
                        </div>
                      </div>
                      <!-- /.box-header -->
                      <div class="box-body text-center">
                        <div class="sparkline" data-type="line" data-spot-Radius="3" data-highlight-Spot-Color="#f39c12" data-highlight-Line-Color="#222" data-min-Spot-Color="#f56954" data-max-Spot-Color="#00a65a" data-spot-Color="#39CCCC" data-offset="90" data-width="100%" data-height="100px" data-line-Width="2" data-line-Color="#39CCCC" data-fill-Color="rgba(57, 204, 204, 0.08)">
                          6,4,7,8,4,3,2,2,5,6,7,4,1,5,7,9,9,8,7,6
                        </div>
                      </div>
                      <!-- /.box-body -->
                    </div>
                    <!-- /.box -->
                  </div>
                  <!-- /.col -->

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


    <!-- Control Sidebar -->
    <aside class="control-sidebar control-sidebar-dark" style="display:none;" id="changemap_aside_apnel">
      <!-- Create the tabs -->
      <ul class="nav nav-tabs nav-justified control-sidebar-tabs">
        <li><a href="#control-sidebar-settings-tab" data-toggle="control-sidebar" onclick="toggleOff();"><i class="fa fa-gears"></i></a></li>
      </ul>

      <!-- Tab panes -->
      <div class="tab-content">

        <!-- Settings tab content -->
        <div class="tab-pane active" id="control-sidebar-settings-tab">
          <form method="post" action="" id="formContent" enctype="multipart/form-data">
            <h3 class="control-sidebar-heading">General Settings</h3>
            <div class="form-group">
              <label class="control-sidebar-subheading">Upload CSV File
                <input type="file" id="csvUpload" name="csvUpload">
                <img id="uploadCSV" src="/images/load.gif" style="display: none;">
                <p class="help-block" id="upload_csv"></p>
                <button class="btn btn-primary pull-right"><i class="fa fa-cloud-upload"></i></button>
              </label>
            </div>
            <!-- /.form-group -->
          </form>
        </div>
        <!-- /.tab-pane -->
       </div>
        <!-- /.tab-content -->
    </aside>
    <!-- /.control-sidebar -->
    <!-- Add the sidebar's background. This div must be placed
         immediately after the control sidebar -->
    <div class="control-sidebar-bg"></div>



  </div>
  <!-- ./wrapper -->

    <script>
      var project_id = <?php echo $project_id ?>;
      var accountid = "<?php echo $_SESSION["accountid"] ?>";
      var accountname = "<?php echo $_SESSION["accountname"] ?>";
      var groupid = "<?php echo $_SESSION["groupid"] ?>";
    </script>

    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/analyzer.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <script>
    function toggleOn()
    {
        $('#changemap_aside_apnel').css('display','block');
    }
    function toggleOff()
    {
        $('#changemap_aside_apnel').css('display','none');
    }
    </script>

</html>
