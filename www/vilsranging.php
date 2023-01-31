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
    <title>SMIMS VILS IoT - Ranging</title>
  </head>

  <div class="row">
      <div class="col-md-12">
          <div class="box">
            <div class="box-header with-border">
              <h3 class="box-title"><?php echo $vilsranging_title;?></h3>
              <button type="button" class="btn btn-box-tool" onclick="toggle_ranging_loading();return false;"><span class="glyphicon glyphicon-pause" id="ranging_state_button"></span></i>
              </button>


              <div class="box-tools pull-right">
                  <!-- <a href="javascript:void(0)" onclick="open_ranging_page();return false;"><i class="fa fa-external-link"></i></a> -->
                  <button type="button" class="btn btn-box-tool" onclick="open_ranging_page();return false;"><i class="fa fa-external-link"></i>
                  </button>
                  <button type="button" class="btn btn-box-tool" data-widget="collapse"><i class="fa fa-minus"></i>
                  </button>
              </div>
            </div>
            <!-- /.box-header -->
            <div class="box-body">

                <!-- orig -->
                <!-- <table class="table table-bordered" id="RANGING_TABLE">
                </table> -->
                <div id="RANGING_TABLE_MSG"><?php echo $vilsranging_loading;?></div>


               <!-- Custom Tabs (Pulled to the right) -->
               <div class="nav-tabs-custom">
                  <ul class="nav nav-tabs" id="RANGING_TABLE_TABS">
                      <!-- tags  -->
                  </ul>
                  <div class="tab-content" id="RANGING_TABLE_CONTENT">
                      <!-- tab-pane -->
                  </div>
                  <!-- /.tab-content -->
               </div>
               <!-- /.nav-tabs-custom -->

             </div>
             <!-- /.box-body -->
          </div>
          <!-- /.box -->
      </div>
      <!--/.col (left) -->
  </div>
  <!--/.row -->


  <script>
    var project_id = <?php echo $project_id ?>;
    var accountid = "<?php echo $_SESSION["accountid"] ?>";
    var accountname = "<?php echo $_SESSION["accountname"] ?>";
    var groupid = "<?php echo $_SESSION["groupid"] ?>";
  </script>

    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/ranging.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/ranging_src/00-ranging.js"></script> -->

</html>
