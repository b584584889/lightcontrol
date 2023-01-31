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
    <title>SMIMS VILS IoT - User Group</title>
  </head>

    <div class="box">
        <div class="box-header with-border">
            <h3 class="box-title"><?php echo $vilsusergroupinfo_title;?></h3>
            <div class="box-tools pull-right">
                <button type="button" class="btn btn-box-tool" onclick="open_usergroupinfo_page();return false;"><i class="fa fa-external-link"></i>
                </button>
            </div>
        </div>
        <!-- /.box-header -->

        <div class="box-body no-padding">
            <table class="table table-bordered">
                <thead>
                <tr style="background-color:#cce6ff;">
                  <th style="width: 10px">#</th>
                  <th><?php echo $vilsusergroupinfo_groupname;?></th>
                  <th><?php echo $vilsusergroupinfo_permission;?></th>
                  <!-- <th><?php echo $vilsusergroupinfo_operation;?></th> -->
                </tr>
                </thead>
                <tbody id="USERGROUPINFO_TABLE">
                </tbody>
            </table>
            <div class="btn-group">
                <!-- <button class="btn btn-block btn-warning btn-lg" id="createnewusergroupbutton" ><i class="fa fa-plus"></i><?php echo $vilsusergroupinfo_creatnewusergroup;?></button> -->
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
    <script src="/js/usergroupinfo.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/usergroupinfo_src/00-usergroupinfo.js"></script> -->

</html>
