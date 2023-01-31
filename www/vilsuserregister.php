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
    <title>SMIMS VILS IoT - User Register</title>
  </head>

    <div class="box">
        <div class="box-header with-border">
            <h3 class="box-title"><?php echo $vilsuserregister_title;?></h3>

            <div class="box-tools pull-right">
                <ul class="pagination pagination-sm no-margin">
                  <li><a href="#" onclick='move_page_backward();return false;'>&laquo;</a></li>
                  <li><a href="#" id="id_current_page_num">1</a></li>
                  <li><a href="#" onclick='move_page_forward();return false;'>&raquo;</a></li>
                </ul>
                <ul class="pagination pagination-sm no-margin pull-right">
                  <li>
                      <button type="button" class="btn btn-box-tool" onclick="open_userregister_page();return false;"><i class="fa fa-external-link"></i>
                      </button>
                  </li>
                </ul>
            </div>
        </div>
        <!-- /.box-header -->

        <div class="box-body no-padding">
            <table class="table table-bordered">
                <thead>
                <tr style="background-color:#cce6ff;">
                  <th style="width: 10px">#</th>
                  <th><?php echo $vilsuserregister_subgroupname;?></th>
                  <th><?php echo $vilsuserregister_email;?></th>
                  <th><?php echo $vilsuserregister_send;?></th>
                  <th><?php echo $vilsuserregister_valid;?></th>
                  <th><?php echo $vilsuserregister_datetime;?></th>
                  <?php
                  if ($GL_CONFIG_EDIT_USERINFO == 1)
                  {
                      echo "<th>"; echo $vilsuserregister_operation; echo "</th>";
                  }
                  ?>
                </tr>
                </thead>
                <tbody id="userregister_TABLE">
                </tbody>
            </table>

        </div>
        <!-- /.box-body -->
    </div>
    <!-- /.box -->

    <script>
      var project_id = <?php echo $project_id ?>;
      var gl_accountid = "<?php echo $_SESSION["accountid"] ?>";
      var gl_accountname = "<?php echo $_SESSION["accountname"] ?>";
      var gl_groupid = "<?php echo $_SESSION["groupid"] ?>";
      var config_enable_edit = <?php echo $GL_CONFIG_EDIT_USERINFO ?>;

    </script>

    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/userregister.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/userregister_src/00-userregister.js?date=123"></script> -->

</html>
