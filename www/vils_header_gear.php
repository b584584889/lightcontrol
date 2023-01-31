<!-- Main Header -->
<header class="main-header">

  <!-- Logo -->
  <a href="/" class="logo">

      <?php
      if($main_show_logo == 1)
      {
          echo "<span class='logo-mini'>";
          echo "   <div class='image'>";
          echo "      <img src='/images/". $main_logo_small_file ."' class='img-circle' alt='VILS Logo' id='logo'>";
          echo "    </div>";
          echo "</span>";

          echo "<span class='logo-lg'>";
          echo "<div class='pull-left image'>";
          echo "  <img src='/images/". $main_logo_file ."' style='width:".$main_logo_width."px;'>";
          echo "</div>";
          echo "</span>";
      }
      else
      {
          echo "<div class='pull-left image'>";
          echo "      <img src='/images/". $main_logo_small_file ."' class='". $main_logo_type ."' alt='VILS Logo'>";
          echo "</div>";
          echo "<span class='logo-mini'><b>". $main_short_text ."</b></span>";
          echo "<span class='logo-lg'><b>". $main_title_prefix_text ."</b> ". $main_title_text ."</b></span>";
      }
      ?>

      <!-- <span class="logo-lg">
          <div class="pull-left image">
              <img src=''/images/auo_logo.jpg' style="width:220px;">
          </div>
      </span> -->
      <!-- mini logo for sidebar mini 50x50 pixels -->
      <!-- <span class="logo-mini""><b>VILS</b></span> -->
      <!-- logo for regular state and mobile devices -->
      <!-- <span class="logo-lg"><b>SMIMS</b> VILS</span> -->
  </a>

  <!-- Header Navbar -->
  <nav class="navbar navbar-static-top" role="navigation">
    <!-- Sidebar toggle button-->
    <a href="#" class="sidebar-toggle" data-toggle="offcanvas" role="button">
      <span class="sr-only">Toggle navigation</span>
    </a>

    <!-- Navbar Right Menu -->
    <div class="navbar-custom-menu">
      <ul class="nav navbar-nav">

        <!--
            Notifications: style can be found in dropdown.less
        -->
        <li class="dropdown notifications-menu">
          <a href="#" class="dropdown-toggle" data-toggle="dropdown">
            <i class="fa fa-bell-o"></i>
            <span class="label label-warning" id="main_top_event_num_bubble" style="display:none">0</span>
          </a>
          <ul class="dropdown-menu">
            <li class="header" id="main_top_event_num">你有 0 个通知</li>
            <li>
              <!-- inner menu: contains the actual data -->
              <div class="firedevent_bubble_content-0"></div>
            </li>
            <li class="footer">
                <a href="javascript:void(0)" onclick="show_eventtimeline(<?php echo $project_id; ?>);return false;"><?php echo $main_lang_seeall; ?></a>
            </li>
          </ul>
        </li>

          <!--
            upload to GAE Server
          -->
          <li class="dropdown messages-menu" style="display:none">
            <a href="#" class="dropdown-toggle" data-toggle="dropdown" id="CONFIG_UPLOAD_RESULT_MENU">
              <i class="fa fa-cloud-upload"></i><span class="hidden-xs">&nbsp;&nbsp;&nbsp;&nbsp;</span>
              <span class="label label-success" id="CONFIG_UPLOAD_RESULT">On</span>
            </a>
            <ul class="dropdown-menu">
                <li class="header">Upload results to VILS IoT Server</li>
                <!-- Menu Footer-->
                <li class="footer">
                  <div class="pull-left">
                    <button type="button" class="btn btn-block btn-success" id="uploadResultOnBtn">On</button>
                  </div>
                  <div class="pull-right">
                    <button type="button" class="btn btn-block btn-danger" id="uploadResultOffBtn">Off</button>
                  </div>
                </li>
            </ul>
          </li>

       <!--
          warning message
       -->
       <li class="dropdown notifications-menu">
         <a href="#" class="dropdown-toggle" data-toggle="dropdown">
           <i class="fa fa-flag-o"></i>
           <span class="label label-danger" id="top_warning_num_bubble" style="display:none">0</span>
         </a>
         <ul class="dropdown-menu">
           <li class="header" id="top_warning_num">You have 0 warnings</li>
           <li>
                 <!-- inner menu: contains the actual data -->
                 <ul class="menu">
                   <li class="top_warning_content_main_websocket">
                   </li>
                   <li class="top_warning_content_alertevent_websocket">
                   </li>
                   <li class="top_warning_content_checkpoint_websocket">
                   </li>

                 </ul>

           </li>

         </ul>
       </li>


       <!--
            projects
        -->
        <li class="dropdown messages-menu" style="<?php if($_SESSION["groupid"] != 1 && $_SESSION["groupid"] != 2 && $_SESSION["groupid"]!=4){echo 'display:none;';} ?>    ">
          <a href="#" class="dropdown-toggle" data-toggle="dropdown">
            <i class="fa fa-map-o" id="curProjectName"></i>
          </a>
          <ul class="dropdown-menu">
            <li class="header" id="totalProjects">You have 0 projects</li>
            <li>
              <!-- inner menu: contains the actual data -->
              <ul class="menu" id="ProjectListMenu">
                <!-- User projects -->
                <!-- end  -->
               </ul>
            </li>
            <li class="footer">
                <a href="#" onclick="create_new_project(this); return false;">Create New Project</a>
            </li>
          </ul>
        </li>

        <!--
             User
         -->
         <li class="dropdown messages-menu">
           <a href="#" class="dropdown-toggle" data-toggle="dropdown">
             <i class="fa fa-user" id="curUserName"></i>
           </a>
           <ul class="dropdown-menu">
               <?php
                   if ($_SESSION["groupid"] == 1 || $_SESSION["groupid"] == 2 || $_SESSION["groupid"] == 4)
                   {
                       echo "<form>";
                       echo "<li>";
                       // echo "<button type='button' class='btn btn-app' onclick='sync_ERP_account();return false;'><i class='fa fa-refresh'></i>同步ERP帳號</button>";
                       //echo "<button type='button' class='btn btn-app' onclick='get_qrcode(\"ikki\");return false;'><i class='fa fa-qrcode'></i>取得 QR Code</button>";
                       echo "<button type='button' class='btn btn-app' onclick='get_qrcode(\"".$_SESSION['accountid']."\",\"".$_SESSION['userid']."\");return false;'><i class='fa fa-qrcode'></i>取得 QR Code</button>";
                       // echo "<button type='button' class='btn btn-app' onclick='get_pdf_report(\"".$_SESSION['accountid']."\");return false;'><i class='fa fa-file-pdf-o'></i>產生報表</button>";
                       echo "</li>";
                       echo "<li>";
                       echo "<img src='' alt='' class='margin' width='150' id='qrcode_ui'>";
                       echo "<div id='pdf_report_dl_link' align='center'><div>";
                       echo "</li>";
                       echo "</form>";
                   }
               ?>
             <li>
                 <span>&emsp;</span>
             </li>
             <li class="footer">
                 <a href="/logout.php" style="background-color:rgb(216, 210, 215);"><i class='fa fa-power-off'></i>Logout</a>
             </li>
           </ul>
         </li>

        <li>
          <div>
            <span id="appversion" style="color:rgb(255, 255, 255);">Version</span><br>
            <span id="apppid" style="color:rgb(255, 255, 255);">PID</span>&nbsp;
        </div>
        </li>
        <!-- Control Sidebar Toggle Button -->
         <?php if($show_gear_button)
         {
             echo '<li>';
             echo '<a href="#control-sidebar-settings-tab" data-toggle="control-sidebar"><i class="fa fa-gears"></i></a>';
             echo '</li>';
         }
         ?>

      </ul>
    </div>

  </nav>

</header>
