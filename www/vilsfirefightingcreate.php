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
    <title><?php echo $firefighting_create; ?></title>
  </head>

<div class="row">

    <div class="col-md-5">
        <div class="box box-primary">
            <div class="box-header with-border">
                <h3 class="box-title"><?php echo $firefighting_create; ?></h3>
                <div class="box-tools pull-right">
                    <button type="button" class="btn btn-box-tool" onclick="open_firefighting_create_page();return false;"><i class="fa fa-external-link"></i>
                    </button>
                </div>
            </div>

            <div class="box-body">
                <div class="form-group">
                  <label for="Address">火場地址</label>
                  <input type="text" class="form-control" name="Address" id="Address" value="">
                  <button class="btn btn-primary pull-left" onclick="findadressloactionbtn_click();return false;">地圖定位</button>
                </div>
            </div>

            <div class="box-footer">
                <div class="pull-center" id="create_msg"></div>
                <button type="submit" class="btn btn-danger pull-right" onclick="createNewFireFightingbtn_click();return false;">新增火場</button>
            </div>

        </div>
    </div>
    <!-- /.col-md-5 -->

    <div class="col-md-7">
        <!-- MAP & BOX PANE -->
        <div class="box box-success">

            <div class="box-header with-border">
                <h3 class="box-title">火場地圖</h3>

                <div class="btn-group">
                    <button type="button" class="btn btn-default btn-sm dropdown-toggle" data-toggle="dropdown" id="cur_address">火場地址<span class="caret"></span></button>
                    <ul class="dropdown-menu" id="dropdown_addresses">
                    </ul>
                </div>

                <div class="btn-group">
                    <div id="Address_longitude">經度:</div>
                </div>
                <div class="btn-group">
                    <div id="Address_latitude">緯度:</div>
                </div>
            </div>

            <div class="box-body" style="height:83vh;">
                <div id="mapid" style="height:100%;"> </div>
            </div>

            <div class="box-footer">
            </div>

          </div>
    </div>
    <!-- /.col-md-7 -->

</div>
<!-- /.row -->

    <script>
      var project_id = <?php echo $project_id ?>;
      var gl_accountid = "<?php echo $_SESSION["accountid"] ?>";
      var gl_accountname = "<?php echo $_SESSION["accountname"] ?>";
      var gl_groupid = "<?php echo $_SESSION["groupid"] ?>";
    </script>

    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/leaflet.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/leaflet_draw.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/firefightingcreate.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/firefightingcreate_src/00-Leaflet.ImageOverlay.Rotated.js"></script>
    <script src="/js/firefightingcreate_src/01-Indoor2DMap.js"></script>
    <script src="/js/firefightingcreate_src/02-create.js"></script> -->

</html>
