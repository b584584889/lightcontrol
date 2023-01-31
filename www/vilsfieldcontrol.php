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
    <title><?php echo $left_side_bar_devices_fieldcontrol; ?></title>
  </head>

<div class="row">
    <div class="col-md-5">
        <div class="box box-primary">
            <div class="box-header with-border">
                <h3 class="box-title">人員狀態</h3>
                <div class="box-tools pull-right">
                    <button type="button" class="btn btn-box-tool" onclick="open_fieldcontrol_page();return false;"><i class="fa fa-external-link"></i>
                    </button>
                </div>
            </div>
        </div>

        <div class="box-body no-padding" style="height:90vh; overflow:auto;">

            <div class="row">
                <div class="col-md-2">
                </div>
                <div class="col-md-10">
                    <div class="btn-group">管制器</div>
                    <div class="btn-group" id="nfclocator_selection"></div>
                </div>
            </div>

            <div class="row">
                <div class="col-md-12">
                    <table class="table table-bordered" id="NODE_LOCATION_TABLE">
                        <thead>
                            <tr>
                                <th style="width: 10px;" class="bg-gray disabled">#</th>
                                <th class="bg-gray disabled">區域名稱</th>
                                <th class="bg-gray disabled">人員</th>
                                <th class="bg-gray disabled">停留時間</th>
                                <th class="bg-gray disabled">氣瓶剩餘量</th>
                                <th class="bg-gray disabled">狀態</th>
                            </tr>
                        </thead>
                        <tbody id="NODE_LOCATION_TABLE_BODY">
                        </tbody>
                    </table>
                </div>
            </div>

            <div class="row">
                <div class="col-md-12">
                    <div id="timeline_footer"></div>
                </div>
            </div>

            <div class="row">
                <div class="col-md-12">
                    <div id="UserActivityDashBoard"></div>
                </div>
            </div>

        </div>

        <div class="box-footer">
        </div>
    </div>
    <div class="col-md-7">
        <!-- MAP & BOX PANE -->
        <div class="box box-success">

              <div class="box-header with-border">
                <h3 class="box-title"><?php echo $vils2dmap_title;?></h3>
                <div class="box-tools pull-right">

                    <div class="btn-group">
                        <input type="text" id="map_search_text" onfocus="map_search_text_onfocus(this.id)" placeholder="尋找裝置">
                        <ul class="dropdown-menu" id="dropdown_searchresult">
                        </ul>
                    </div>

                    <div class="btn-group">
                        <button type="button" class="btn btn-default btn-sm dropdown-toggle" data-toggle="dropdown"><?php echo $vils2dmap_show_locators;?><span class="caret"></span></button>
                        <ul class="dropdown-menu" id="dropdown_locators">
                        </ul>
                    </div>
                    <div class="btn-group">
                        <button type="button" class="btn btn-default btn-sm dropdown-toggle" data-toggle="dropdown"><?php echo $vils2dmap_show_tags;?><span class="caret"></span></button>
                        <ul class="dropdown-menu" id="dropdown_tags">
                        </ul>
                    </div>

                </div>
              </div>
              <!-- /.box-header -->
              <div class="box-body no-padding" style="height:83vh;">
                  <!-- Custom Tabs (Pulled to the right) -->
                  <div class="nav-tabs-custom" style="height:100%;">

                    <ul class="nav nav-tabs" id="map_nav_tabs" style="height:10%;">
                        <!--

                        Show PathGroup Info Here

                        -->
                    </ul>
                    <div class="tab-content" id="map_tab_content" style="height:90%;">
                        <!--

                        Show All Path Info Here

                        -->
                    </div>
                    <!-- /.tab-content -->

                  </div>
                  <!-- nav-tabs-custom -->
              </div>
              <!-- /.box-body -->
              <div class="box-footer">
                  <div class="btn-group">
                      <div class="checkbox">
                        <label>
                          <input type="checkbox" id="showtrack"> Show Track
                        </label>
                      </div>
                      <div class="checkbox">
                        <label>
                          <input type="checkbox" id="uismoothingcheckbtn" checked="true"> UI Smoothing
                        </label>
                      </div>
                  </div>
             </div>
        </div>
        <!-- /.box box-success -->
    </div>
    <!-- /.col-md-12 -->
</div>
<!-- /.row -->

    <script>
      var project_id = <?php echo $project_id ?>;
      var gl_accountid = "<?php echo $_SESSION["accountid"] ?>";
      var gl_accountname = "<?php echo $_SESSION["accountname"] ?>";
      var gl_groupid = "<?php echo $_SESSION["groupid"] ?>";

      var show_nodeid = "<?php echo $nodeid ?>";
      var GL_CONFIG_EDIT_2DMAP = <?php echo $GL_CONFIG_EDIT_2DMAP ?>;
      var GL_CONFIG_2DMAP_SHOW_GMAP = <?php echo $GL_CONFIG_2DMAP_SHOW_GMAP ?>;

    </script>

    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/leaflet.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/leaflet_draw.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <!-- <script src="/js/fieldcontrol.min.js?date=<?php echo $UPDATE_DATE;?>"></script> -->

    <script src="/js/fieldcontrol_src/00-raphael.js"></script>
    <script src="/js/fieldcontrol_src/01-morris.js"></script>
    <script src="/js/fieldcontrol_src/02-jquery.knob.js"></script>
    <script src="/js/fieldcontrol_src/03-Leaflet.ImageOverlay.Rotated.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/fieldcontrol_src/04-mapapi.js"></script>
    <script src="/js/fieldcontrol_src/05-Indoor2DMap.js"></script>
    <script src="/js/fieldcontrol_src/06-LiveMap.js"></script>
    <script src="/js/fieldcontrol_src/07-fieldcontrol.js?date=<?php echo $UPDATE_DATE;?>"></script>

</html>
