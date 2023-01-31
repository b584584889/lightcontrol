<?php
include ('checksession.php');
?>

<!DOCTYPE HTML>
<html lang="en">
    <?php
    include ('vi_html_head.php');
    ?>

    <div class="box">
        <div class="box-header with-border">
            <h3 class="box-title"><?php echo $vilsusersubgroupinfo_title;?></h3>
            <div class="box-tools pull-right">
                <ul class="pagination pagination-sm no-margin">
                  <li><a href="#" onclick='move_page_backward();return false;'>&laquo;</a></li>
                  <li><a href="#" id="id_current_page_num">1</a></li>
                  <li><a href="#" onclick='move_page_forward();return false;'>&raquo;</a></li>
                </ul>
            </div>
        </div>
        <!-- /.box-header -->

        <div class="box-body no-padding">
            <table class="table table-bordered">
                <thead>
                <tr style="background-color:#cce6ff;">
                  <th style="width: 10px">#</th>
                  <th><?php echo $vilsusersubgroupinfo_groupname;?></th>
                  <th><?php echo $vilsusersubgroupinfo_permission;?></th>
                  <?php
                      echo "<th>"; echo $vilsusersubgroupinfo_operation; echo "</th>";
                  ?>
                </tr>
                </thead>
                <tbody id="usersubgroupinfo_TABLE">
                </tbody>
            </table>
            <div class="btn-group">
                <?php
                    echo "<button class='btn btn-block btn-warning btn-lg' id='createnewusersubgroupbutton' ><i class='fa fa-plus'></i>"; echo $vilsusersubgroupinfo_creatnewusergroup; echo"</button>";
                ?>
            </div>

        </div>
        <!-- /.box-body -->
    </div>
    <!-- /.box -->

    <div style="text-align:center">
        <button class="btn btn-default btn-flat" onclick="location.href='/di'" style="display:block;display:table;margin:0 auto;"><i class="fa fa-reply"></i>返回</button>
    </div>

    <script>
      var project_id = 1;
      var accountid = "<?php echo $_SESSION["accountid"] ?>";
      var accountname = "<?php echo $_SESSION["accountname"] ?>";
      var groupid = "<?php echo $_SESSION["groupid"] ?>";
      var config_enable_edit = 1;

      console.log("accountid:"+accountid);

    </script>

    <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
    <script src="/js/usersubgroupinfo.min.js?date=<?php echo $UPDATE_DATE;?>"></script>

    <!-- <script src="/js/usersubgroupinfo_src/00-usersubgroupinfo.js"></script> -->

</html>
