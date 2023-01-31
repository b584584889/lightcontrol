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
    <title>SMIMS VILS IoT - Location</title>
  </head>

    <div class="box">
        <div class="box-header with-border">
            <h3 class="box-title"><?php echo $vilslocation_title;?></h3>
            <div class="box-tools pull-right">
                <!-- <a href="javascript:void(0)" onclick="open_location_page();return false;"><i class="fa fa-external-link"></i></a> -->
                <button type="button" class="btn btn-box-tool" onclick="open_location_page();return false;"><i class="fa fa-external-link"></i>
                </button>
            </div>
        </div>
        <!-- /.box-header -->

        <div class="box-body">
            <table class="table table-bordered" id="NODE_LOCATION_TABLE">
                <thead>
                    <tr>
                        <th style="width: 10px;" class="bg-gray disabled">#</th>
                        <th style="width: 120px;" class="bg-gray disabled"><?php echo $vilslocation_location;?></th>
                        <th style="width: 80px;" class="bg-gray disabled"><?php echo $vilslocation_type;?></th>
                        <th style="width: 10px;" class="bg-gray disabled"><?php echo $vilslocation_activity;?></th>
                        <th class="bg-gray disabled"><?php echo $vilslocation_device;?></th>
                    </tr>
                </thead>
                <tbody id="NODE_LOCATION_TABLE_BODY">
                </tbody>
            </table>
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
    <script src="/js/location.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/location_src/00-location.js"></script> -->

</html>
