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
    <title>SMIMS VILS IoT - Activity</title>
  </head>

              <div class="box">
                <div class="box-header with-border">
                  <h3 class="box-title"><?php echo $vilsactivity_title;?></h3>
                  <div class="box-tools pull-right">
                      <!-- <a href="javascript:void(0)" onclick="open_activity_page();return false;"><i class="fa fa-external-link"></i></a> -->
                      <button type="button" class="btn btn-box-tool" onclick="open_activity_page();return false;"><i class="fa fa-external-link"></i>
                      </button>
                      <button type="button" class="btn btn-box-tool" data-widget="collapse"><i class="fa fa-minus"></i>
                      </button>
                  </div>
                </div>
                <!-- /.box-header -->
                <div class="box-body">

                  <!-- Custom Tabs (Pulled to the right) -->
                  <div class="nav-tabs-custom">
                    <ul class="nav nav-tabs">
                      <li class="active"><a href="#loc_tab_tags" data-toggle="tab"><span class="badge bg-blue-active"><?php echo $vilsactivity_tag;?></span></a></li>
                      <li><a href="#loc_tab_anchors" data-toggle="tab"><span class="badge bg-green-active"><?php echo $vilsactivity_anchor;?></span></a></li>
                      <li><a href="#loc_tab_coords" data-toggle="tab"><span class="badge bg-red-active"><?php echo $vilsactivity_coordinator;?></span></a></li>
                      <li><a href="#loc_tab_locators" data-toggle="tab"><span class="badge bg-yellow-active"><?php echo $vilsdevices_locator;?></span></a></li>
                      <li><a href="#loc_tab_sensors" data-toggle="tab"><span class="badge bg-purple-active"><?php echo $vilsdevices_sensors;?></span></a></li>
                      <li><a href="#loc_tab_beacon" data-toggle="tab"><span class="badge bg-aqua-active"><?php echo $vilsdevices_beacon;?></span></a></li>
                    </ul>
                    <div class="tab-content">
                      <div class="tab-pane active table-responsive" id="loc_tab_tags">
                          <table class="table table-bordered">
                            <thead>
                              <tr>
                                <th style="width: 10px">#</th>
                                <th style="width: 150px"><?php echo $vilsactivity_typeindex;?></th>
                                <th style="width: 100px"><?php echo $vilsactivity_macaddress;?></th>
                                <th style="width: 40px"><?php echo $vilsactivity_timestamp;?></th>
                                <th><?php echo $vilsactivity_activity;?></th>
                              </tr>
                            </thead>
                            <tbody id="TAG_NODE_ACTIVITY_TABLE_BODY">
                            </tbody>
                          </table>
                      </div>
                      <!-- /.tab-pane -->
                      <div class="tab-pane table-responsive" id="loc_tab_anchors">
                          <table class="table table-bordered">
                            <thead>
                              <tr>
                                <th style="width: 10px">#</th>
                                <th style="width: 150px"><?php echo $vilsactivity_typeindex;?></th>
                                <th style="width: 100px"><?php echo $vilsactivity_macaddress;?></th>
                                <th style="width: 40px"><?php echo $vilsactivity_timestamp;?></th>
                                <th><?php echo $vilsactivity_activity;?></th>
                              </tr>
                            </thead>
                            <tbody id="ANCHOR_NODE_ACTIVITY_TABLE_BODY">
                            </tbody>
                          </table>
                      </div>
                      <!-- /.tab-pane -->
                      <div class="tab-pane table-responsive" id="loc_tab_coords">
                          <table class="table table-bordered">
                            <thead>
                              <tr>
                                <th style="width: 10px">#</th>
                                <th style="width: 150px"><?php echo $vilsactivity_typeindex;?></th>
                                <th style="width: 100px"><?php echo $vilsactivity_macaddress;?></th>
                                <th style="width: 40px"><?php echo $vilsactivity_timestamp;?></th>
                                <th><?php echo $vilsactivity_activity;?></th>
                              </tr>
                            </thead>
                            <tbody id="COORD_NODE_ACTIVITY_TABLE_BODY">
                            </tbody>
                          </table>
                      </div>
                      <!-- /.tab-pane -->

                      <div class="tab-pane table-responsive" id="loc_tab_locators">
                          <table class="table table-bordered">
                            <thead>
                              <tr>
                                <th style="width: 10px">#</th>
                                <th style="width: 150px"><?php echo $vilsactivity_typeindex;?></th>
                                <th style="width: 100px"><?php echo $vilsactivity_macaddress;?></th>
                                <th style="width: 40px"><?php echo $vilsactivity_timestamp;?></th>
                                <th><?php echo $vilsactivity_activity;?></th>
                              </tr>
                            </thead>
                            <tbody id="LOCATOR_NODE_ACTIVITY_TABLE_BODY">
                            </tbody>
                          </table>
                      </div>
                      <!-- /.tab-pane -->

                      <div class="tab-pane table-responsive" id="loc_tab_sensors">
                          <table class="table table-bordered">
                            <thead>
                              <tr>
                                <th style="width: 10px">#</th>
                                <th style="width: 150px"><?php echo $vilsactivity_name;?></th>
                                <th style="width: 120px"><?php echo $vilsactivity_timestamp;?></th>
                                <th><?php echo $vilsactivity_activity;?></th>
                              </tr>
                            </thead>
                            <tbody id="SENSOR_NODE_ACTIVITY_TABLE_BODY">
                            </tbody>
                          </table>
                      </div>
                      <!-- /.tab-pane -->

                    <div class="tab-pane table-responsive" id="loc_tab_beacon">
                        <table class="table table-bordered">
                          <thead>
                            <tr>
                              <th style="width: 10px">#</th>
                              <th style="width: 150px"><?php echo $vilsactivity_name;?></th>
                              <th style="width: 120px"><?php echo $vilsactivity_timestamp;?></th>
                              <th><?php echo $vilsactivity_distance;?></th>
                            </tr>
                          </thead>
                          <tbody id="BEACON_NODE_ACTIVITY_TABLE_BODY">
                          </tbody>
                        </table>
                    </div>
                    <!-- /.tab-pane -->

                    </div>
                    <!-- /.tab-content -->

                  </div>
                  <!-- nav-tabs-custom -->



                  <!-- <table class="table table-bordered" id="NODE_ACTIVITY_TABLE">
                    <thead>
                      <tr>
                        <th style="width: 10px">#</th>
                        <th style="width: 150px">Type/Index</th>
                        <th style="width: 100px">MAC Address</th>
                        <th style="width: 40px">Update Time</th>
                        <th>Activity</th>
                      </tr>
                    </thead>
                    <tbody id="NODE_ACTIVITY_TABLE_BODY">
                    </tbody>
                  </table> -->


                </div>
                <!-- /.box-body -->
              </div>
              <!-- /.box -->


    <script>
        var project_id = <?php echo $project_id ?>;
        var accountid = "<?php echo $_SESSION["accountid"] ?>";
        var accountname = "<?php echo $_SESSION["accountname"] ?>";
        var groupid = "<?php echo $_SESSION["groupid"] ?>";
    </script>


    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/activity.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/activity_src/00-activity.js"></script>
    <script src="/js/activity_src/01-sensorinfo.js"></script> -->

</html>
