<?php
include ('checksession.php');
?>

<!DOCTYPE HTML>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, minimum-scale=1.0, user-scalable=yes">
    <link rel="shortcut icon" sizes="16x16 32x32 48x48 64x64" href="/images/favicon.ico">
    <link href="/css/vils.min.css" rel="stylesheet">
    <meta name="description" content="SMIMS VILS IoT">
    <meta name="author" content="SMIMS">
    <title>SMIMS VILS IoT - Footprint</title>
  </head>

            <!-- MAP & BOX PANE -->
            <div class="box box-success">
              <div class="box-header with-border">
                <h3 class="box-title"><?php echo $vilsfootprint_title;?></h3>
                <div class="box-tools pull-right">
                    <button type="button" class="btn btn-box-tool" onclick="open_footprint_page();return false;"><i class="fa fa-external-link"></i>
                    </button>
                    <!-- <a href="javascript:void(0)" onclick="open_footprint_page();return false;"><i class="fa fa-external-link"></i></a> -->
                    <button type="button" class="btn btn-box-tool" data-widget="collapse"><i class="fa fa-minus"></i>
                    </button>
                </div>
              </div>
              <!-- /.box-header -->
              <div class="box-body no-padding">
                <div class="row">
                  <div class="col-md-12 col-sm-12">
                    <div class="pad">
                      <!-- Map will be created here -->
                      <div id="mapid" style="height: 50vh"></div>
                    </div>
                  </div>
                  <!-- /.col -->
                </div>
                <!-- /.row -->

                <div class="row">
                  <div class="col-md-12 col-sm-12">
                    <div class="pad">
                    <div class="button-group">
                      <button type="button" class="btn btn-default btn-sm dropdown-toggle" data-toggle="dropdown"><?php echo $vilsfootprint_select_tag;?><span class="caret"></span></button>
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
                      <div id="visualization" style="height:auto"></div>
                      <!-- <button onclick="showAllGroups()">Restore Hidden</button> -->
                      <div id="customTime" style="text-align:center">&nbsp;</div>
                      <div id="messagelog" style="display:none">&nbsp;</div>
                    </div>
                  </div>
                  <!-- /.col -->
                </div>
                <!-- /.row -->

                <div class="row">
                  <div class="col-md-12 col-sm-12">
                      <div class="pad col-md-offset-3">
                      <a class="btn btn-app" id="FootprintPauseBtn">
                        <i class="fa fa-pause"></i> <?php echo $vilsfootprint_pause;?>
                      </a>
                      <a class="btn btn-app" id="FootprintPlayBtn">
                        <i class="fa fa-play"></i> <?php echo $vilsfootprint_play;?>
                      </a>
                      <div class="btn-group">
                        <button type="button" class="btn btn-default btn-flat" id="FootprintPlaySpeed"><?php echo $vilsfootprint_speed_1;?></button>
                        <button type="button" class="btn btn-default btn-flat dropdown-toggle" data-toggle="dropdown">
                          <span class="caret"></span>
                          <span class="sr-only">Toggle Dropdown</span>
                        </button>
                        <ul class="dropdown-menu" role="menu">
                          <li><a href="javascript:;" onclick="setPlaybackspeed(1);return false;"><?php echo $vilsfootprint_speed_1;?></a></li>
                          <li><a href="javascript:;" onclick="setPlaybackspeed(4);return false;"><?php echo $vilsfootprint_speed_4;?></a></li>
                          <li><a href="javascript:;" onclick="setPlaybackspeed(16);return false;"><?php echo $vilsfootprint_speed_16;?></a></li>
                          <li><a href="javascript:;" onclick="setPlaybackspeed(60);return false;"><?php echo $vilsfootprint_speed_60;?></a></li>
                         </ul>
                        </div>
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


    <script>
        var project_id = <?php echo $project_id ?>;
        var accountid = "<?php echo $_SESSION["accountid"] ?>";
        var accountname = "<?php echo $_SESSION["accountname"] ?>";
        var groupid = "<?php echo $_SESSION["groupid"] ?>";
    </script>

    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_footprint.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

</html>
