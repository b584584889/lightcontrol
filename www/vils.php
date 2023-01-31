<?php
include ('checksession.php');
?>

<!DOCTYPE HTML>
<html lang="en">
    <?php
    include ('vils_html_head.php');

    if ($GL_CONFIG_ENABLE_MIN_SIDEBAR == 1)
    {
        echo '<body class="hold-transition skin-blue sidebar-collapse sidebar-mini">';
    }
    else
    {
        echo '<body class="hold-transition skin-blue sidebar-mini">';
    }
    ?>
  <!-- <body class="hold-transition skin-blue sidebar-collapse sidebar-mini"> -->


    <div class="wrapper">

    <?php
        $show_gear_button = false;
        include ('vils_header_gear.php');

        $target_page = 'vilsdashboard';
        if ($GL_CONFIG_ENABLE_DASHBOARD == 1)
        {
            $dashboard_active = true;
            $target_page = 'vilsdashboard';
        }
        else
        if ($GL_CONFIG_ENABLE_NODELIST == 1)
        {
            $vils_active = true;
            $target_page = 'vilsdevices';
        }
        else
        if ($GL_CONFIG_ENABLE_USEINFO == 1)
        {
            $userinfo_active = true;
            $target_page = 'vilsuserinfo';
        }
        else
        {
            $vils2dmap_active = true;
            $target_page = 'vils2dmap';
        }

        include ('left_side_bar.php');

    ?>

    <!-- Content Wrapper. Contains page content -->
    <div class="content-wrapper">

    <!-- Main content -->
    <section class="content" style="margin:0; padding:10; border:0 none;">
        <div class="row">
              <!-- Left col -->
              <div class="col-md-8 col-sm-8" id="main_left_frame">

                <iframe id="contentIframe" src="/index.php/<?php echo $target_page ?>/<?php echo $project_id ?>" onLoad="" style="width:100%; height:85vh; margin:0; padding:0; border:0 none;" ></iframe>

              </div>
              <!-- /.Left col -->

              <!-- right column -->
              <div class="col-md-4 col-sm-4" id="main_right_frame">
                <iframe id="rightcontentIframe" src="/index.php/vilsalerteventtimeline/<?php echo $project_id ?>" onLoad="" style="width:100%; height:85vh; margin:0; padding:0; border:0 none;" ></iframe>
              </div>
              <!--/.col (right) -->
        </div>
        <!-- /.row -->

    </section>
    <!-- /.content -->
  </div>
  <!-- /.content-wrapper -->


    <!-- Main Footer -->
    <footer class="main-footer">
      <!-- To the right -->
      <div class="pull-right hidden-xs">
      &nbsp;系統時間:<span id="system_time"></span>
      </div>

      <div class="pull-right hidden-xs">
      <strong> <?php echo $main_bottom_title ?></strong>&nbsp;&nbsp;v<?php echo $web_version ?>
      </div>

      <!-- Default to the left -->
      <div>
          <strong><?php echo $main_copyright_name ?></strong> <?php echo $main_copyright ?>
      </div>
    </footer>

  </div>
  <!-- ./wrapper -->

  <script>
    var project_id = <?php echo $project_id ?>;
    //console.log("vils.php project_id="+project_id);
    var accountid = "<?php echo $_SESSION["accountid"] ?>";
    var accountname = "<?php echo $_SESSION["accountname"] ?>";
    var groupid = "<?php echo $_SESSION["groupid"] ?>";

    var type = "<?php echo $_SESSION["type"] ?>";
    var userid = "<?php echo $_SESSION["userid"] ?>";
    var gl_maingroups = "<?php echo $_SESSION['maingroups'] ?>";
    console.log("vils.php accountid:"+accountid+", accountname:"+accountname+", project_id:"+project_id+", groupid:"+groupid+", gl_maingroups:"+gl_maingroups+", type:"+type+", userid:"+userid);
  </script>

  <script src="/js/global.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
  <script src="/js/map_base.min.js?date=<?php echo $UPDATE_DATE;?>"></script>
  <script src="/js/vils_src/00-vils.js?date=<?php echo date('Y-m-d H:i:s');?>"></script>
  <!-- <script src="/js/vils.min.js?date=<?php echo $UPDATE_DATE;?>"></script> -->

  <!-- <script src="/js/vils_src/00-vils.js?date=<?php echo $UPDATE_DATE;?>"></script>
  <script src="/js/vils_src/01-checkeventfired.js?date=<?php echo $UPDATE_DATE;?>"></script> -->

</body>

</html>
