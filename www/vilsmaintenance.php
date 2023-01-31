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
    <title>SMIMS VILS IoT - Device Maintenance Record</title>
  </head>

    <div class="box">
        <div class="box-header with-border">
            <h3 class="box-title"><?php echo $vilsmaintenance_title;?></h3>
            <div class="box-tools pull-right">
                <button type="button" class="btn btn-box-tool" onclick="open_maintenance_page();return false;"><i class="fa fa-external-link"></i>
                </button>
            </div>
        </div>
        <!-- /.box-header -->

        <div class="box-body">
            <table class="table table-bordered" id="MAINTENANCE_TABLE">
                <thead>
                    <tr>
                        <th style="width: 10px;" class="bg-gray disabled">#</th>
                        <th class="bg-gray disabled"><?php echo $vilsmaintenance_name;?></th>
                        <th class="bg-gray disabled"><?php echo $vilsmaintenance_type;?></th>
                        <th class="bg-gray disabled"><?php echo $vilsmaintenance_unit;?></th>
                        <th class="bg-gray disabled"><?php echo $vilsmaintenance_manager;?></th>
                        <th class="bg-gray disabled"><?php echo $vilsmaintenance_assetlife;?></th>
                        <th style="width: 150px;" class="bg-gray disabled"><?php echo $vilsmaintenance_maintenancedate;?></th>
                        <th class="bg-gray disabled"><?php echo $vilsmaintenance_status;?></th>
                        <th style="width: 150px;" class="bg-gray disabled"><?php echo $vilsmaintenance_datetime;?></th>
                    </tr>
                </thead>
                <tbody id="MAINTENANCE_TABLE_BODY">
                </tbody>
            </table>
            <div class="btn-group">
                <?php
                if ($GL_CONFIG_EDIT_MAINTENANCE)
                {
                    echo "<button class='btn btn-block btn-warning btn-lg' id='createnewmaintenancebutton' ><i class='fa fa-plus'></i>"; echo $vilsmaintenance_createnewrecord; echo"</button>";
                }
                ?>
            </div>
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
    <script src="/js/maintenance.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/maintenance_src/00-maintenance.js?date=<?php echo $UPDATE_DATE;?>"></script> -->


</html>
