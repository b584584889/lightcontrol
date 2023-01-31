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
    <title>SMIMS VILS IoT - 2D Map DashBoard</title>
  </head>

<div class="row">
    <div class="col-md-4">
        <div class="box box-primary">
            <div class="box-header with-border">
                <h3 class="box-title">人員狀態</h3>
            </div>
        </div>

        <div class="box-body no-padding" style="height:90vh; overflow:auto;">

            <div id="UserActivityDashBoard"></div>

        </div>

        <div class="box-footer">

        </div>
    </div>
    <div class="col-md-8">
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
    <script src="/js/map_dashboard.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/map_dashboard_src/00-raphael.js"></script>
    <script src="/js/map_dashboard_src/01-morris.js"></script>
    <script src="/js/map_dashboard_src/02-jquery.knob.js"></script>
    <script src="/js/map_dashboard_src/03-Leaflet.ImageOverlay.Rotated.js"></script>
    <script src="/js/map_dashboard_src/04-mapapi.js"></script>
    <script src="/js/map_dashboard_src/05-Indoor2DMap.js"></script>
    <script src="/js/map_dashboard_src/06-LiveMap.js"></script> -->

</html>
