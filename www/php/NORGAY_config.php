<?php
    require_once("SMIMS_config.php");

    $ALERT_BOX_FONT_SIZE = 3;

    $login_tilte = '諾格科技</br><label style="color:#ff0000">NFC For Everyone</label>';
    $login_sign_in_comment = '請登入帳號';
    $login_sign_in_account = '帳號';
    $login_sign_in_password = '密碼';
    $login_sign_in_userid = '識別碼(選填)';
    $login_sign_in_loginbtn = '登入';

    $register_new_account = '申請新帳號';
    $register_new_groupname = '子集合名稱';
    $register_groupname_explain = '子集合名稱用來區別使用場合或區域:如xx補習班';
    $register_new_email = 'Email(登入帳號)';
    $register_new_password = '密碼';
    $register_retype_password = '再輸入一次密碼';
    $register_apply = '註冊';

    $activate_account = '啟動帳號';
    $activate_reregister = '重新註冊';
    $activate_login = '登入網站';

    $unlockpage_tilte1 = '請購買點名器';
    $unlockpage_tilte2 = '以啟用此功能!';
    $unlockpage_buy = '購買';
    $unlockpage_verifykey = '機器碼(共8碼)';
    $unlockpage_registerdevice = '新增點名器 ';

    $page_tilte = 'Norgay NFC For Everyone';
    $main_show_logo = true;
    $main_logo_file = 'Norgay_logo_2_l.png';
    $main_logo_small_file = 'Norgay_logo_s.png';
    $main_logo_width = '220';
    $main_logo_type = 'img-circle';
    $main_short_text = 'VILS';
    $main_title_prefix_text = 'SMIMS';
    $main_title_text = 'VILS';
    $main_bottom_title = 'SMIMS VILS IPS Solutions';
    $main_copyright_name = 'Copyright &copy; 2021 <a href="http://smims.com.tw">SMIMS Corp</a>.';
    $main_copyright = 'All rights reserved.';

    $main_lang_seeall = '看全部';

    $left_side_bar_dashboard_list = '系統概覽';
    $left_side_bar_devices = '裝置';
    $left_side_bar_devices_list = '裝置管理';
    $left_side_bar_rfiddevices_list = 'RFID裝置管理';
    $left_side_bar_rfiddevices = 'RFID裝置列表';
    $left_side_bar_rfidcards = 'RFID卡片列表';
    $left_side_bar_userinfo = '人員管理';
    $left_side_bar_userlist = '人員列表';
    $left_side_bar_usergroupinfo = '權限群組';
    $left_side_bar_usersubgroupinfo = '子集合';
    $left_side_bar_usermaingroupinfo = '主集合';
    $left_side_bar_cardrollcall = '卡片點名';
    $left_side_bar_cardrollcall_schedule = '點名排程';
    $left_side_bar_cardrollcall_event = '點名排程紀錄';
    $left_side_bar_cardrollcall_record = '卡片點名紀錄';
    $left_side_bar_userrollcall_record = '人員點名紀錄';
    $left_side_bar_cardscan_result = '卡片感應結果';
    $left_side_bar_nfc_cardscan_result = 'NFC卡片感應結果';
    $left_side_bar_uhf_cardscan_result = 'UHF卡片感應結果';
    $left_side_bar_bodyinfo = '生理量測';
    $left_side_bar_bodyinfo_record = '生理量測紀錄';
    $left_side_bar_bodyinfo_manual_input = '手動輸入紀錄';
    $left_side_bar_foracare_bodyinfo = '第三方裝置即時量測資訊';
    $left_side_bar_pushnotification = '推播訊息';
    $left_side_bar_devices_location = '區域';
    $left_side_bar_devices_ranging = '測距結果';
    $left_side_bar_devices_2d = '2D 地圖';
    $left_side_bar_devices_3d = '3D 地圖';
    $left_side_bar_devices_activity = '物聯網';
    $left_side_bar_devices_maintenance = '裝置履歷';

    $left_side_bar_workassign = '工作指示';
    $left_side_bar_survey = '待辦事項';
    $left_side_bar_survey_records = '待辦事項紀錄';
    $left_side_bar_assist = '協助事項';
    $left_side_bar_assist_records = '協助紀錄';

    $left_side_bar_billboard = '公佈欄';
    $left_side_bar_billboard_create = '發新公告';
    $left_side_bar_billboard_records = '公告事項';

    $left_side_bar_courses = '團體活動';
    $left_side_bar_courses_list = '團體活動表';

    $left_side_bar_weekschedule = '功課表';
    $left_side_bar_weekschedule_list = '班級功課表';

    $left_side_bar_applicationform = '表單';
    $left_side_bar_applicationform_leave = '請假單';

    $left_side_bar_footprint = '足跡';
    $left_side_bar_footprint_tags = '標籤足跡';
    $left_side_bar_footprint_courses = '課程記錄';
    $left_side_bar_footprint_areas = '區域記錄';
    $left_side_bar_footprint_inoutarea = '進出區域';
    $left_side_bar_footprint_GPS = 'GPS足跡';
    $left_side_bar_footprint_list = '足跡記錄';
    $left_side_bar_footprint_history = '歷史足跡';

    $left_side_bar_event = '事件';
    $left_side_bar_event_llist = '事件列表';

    $left_side_bar_filestorage = '檔案庫';
    $left_side_bar_filestorage_image = '相簿編輯';
    $left_side_bar_filestorage_file = '檔案編輯';

    $left_side_bar_setting = '設定';
    // $left_side_bar_setting_checkpoint = '巡檢路線';
    // $left_side_bar_setting_checkpoint_setting = '巡檢路線設定';
    // $left_side_bar_setting_checkpoint_dataview = '每日記錄';
    // $left_side_bar_setting_checkpoint_history = '歷史記錄';
    $left_side_bar_setting_inspection_tree = '巡檢';
    $left_side_bar_setting_inspection = '巡檢路線';
    $left_side_bar_setting_inspection_report = '巡檢報告';
    $left_side_bar_setting_inspection_setting = '巡檢路線設定';
    $left_side_bar_setting_inspection_dataview = '每日記錄';
    $left_side_bar_setting_inspection_history = '歷史記錄';

    $left_side_bar_setting_section_tree = '區段';
    $left_side_bar_setting_section = '區段設定';

    $left_side_bar_setting_fencealert_tree = '電子圍籬';
    $left_side_bar_setting_fencealert = '電子圍籬';

    $left_side_bar_setting_system = '系統設定';
    $left_side_bar_setting_di = 'DI卡片項目設定';
    $left_side_bar_setting_pushnotify = '推播項目設定';

    $left_side_bar_EM = 'Engineering Mode';
    $left_side_bar_EM_Diagnosis = 'Diagnosis';
    $left_side_bar_EM_Analyzer = 'Analyzer';

    $vilsdevices_devicelist = '裝置列表';
    $vilsdevices_tag = 'UWB卡片';
    $vilsdevices_nfccard = 'RFID_NFC卡片';
    $vilsdevices_uhfcard = 'RFID_UHF卡片';
    $vilsdevices_unknown_nfccard = 'NFC未註冊卡片';
    $vilsdevices_unknown_uhfcard = 'UHF未註冊卡片';
    $vilsdevices_anchor = '基站';
    $vilsdevices_coordinator = '閘道';
    $vilsdevices_locator = 'LoRa Locator';
    $vilsdevices_celllocator = 'Cellular Locator';
    $vilsdevices_wifilocator = 'WIFI Locator';
    $vilsdevices_edgecontrol = 'Edge Control';
    $vilsdevices_nfclocator = 'NFC感應器';
    $vilsdevices_uhfreader = 'UHF感應器';
    $vilsdevices_showfilter = '顯示項目';
    $vilsdevices_filter_showonline = '在線裝置';
    $vilsdevices_filter_showoffline = '離線裝置';

    $vilsdevices_loragateway = 'LoRA Gateway';

    $vilsdevices_alive = '活動';
    $vilsdevices_ping = '網路';
    $vilsdevices_MQTT = 'MQTT';
    $vilsdevices_deviceid = '裝置序號';
    $vilsdevices_devicename = '裝置名稱';
    $vilsdevices_typeindex = '形式/編號';
    $vilsdevices_role = '角色';
    $vilsdevices_buttontype = '按鈕型態';
    $vilsdevices_resident = '住戶';
    $vilsdevices_user = '人員';
    $vilsdevices_macaddress = '硬體位址';
    $vilsdevices_position = '座標';
    $vilsdevices_sensoralert = '入侵警示';
    $vilsdevices_GPS = 'GPS';
    $vilsdevices_maplayer = '圖層';
    $vilsdevices_building = '樓號';
    $vilsdevices_floor = '樓層';
    $vilsdevices_info = '資訊';
    $vilsdevices_available_anchors = '可用基站';
    $vilsdevices_projects = '可用專案';
    $vilsdevices_IP = 'IP';
    $vilsdevices_version = '版本';
    $vilsdevices_PID = 'PID';
    $vilsdevices_CoordRunTime = '運行時間';
    $vilsdevices_blacklist = '黑名單';
    $vilsdevices_whitelist = '白名單';
    $vilsdevices_joinRangingRange = 'Anchor可Join且可測距範圍';
    $vilsdevices_joinNotRangingRange = 'Anchor可Join但不可測距範圍';
    $vilsdevices_reset = '重置';
    $vilsdevices_LoRA = 'LoRA位址';
    $vilsdevices_general = '一般設定';
    $vilsdevices_editinfo = '編輯裝置資訊';
    $vilsdevices_toggle = '切換';
    $vilsdevices_loading = '載入中...';
    $vilsdevices_gwid = 'gwid';
    $vilsdevices_sf = 'sf';
    $vilsdevices_repeater = 'repeater';
    $vilsdevices_channel = 'channel';
    $vilsdevices_nfc_alive = '活動';
    $vilsdevices_nfc_cardname = '卡片名稱';
    $vilsdevices_nfc_cardtype = '卡片型態';
    $vilsdevices_nfc_id = '卡片ID';
    $vilsdevices_nfc_maplayer = '圖層';
    $vilsdevices_uhf_alive = '活動';
    $vilsdevices_uhf_cardname = '卡片名稱';
    $vilsdevices_uhf_cardtype = '卡片型態';
    $vilsdevices_uhf_id = '卡片ID';
    $vilsdevices_uhf_maplayer = '圖層';
    $vilsdevices_nfclocator_alive = '活動';
    $vilsdevices_nfclocator_name = '裝置名稱';
    $vilsdevices_nfclocator_type = '機型';
    $vilsdevices_nfclocator_verifykey = '機器碼';
    $vilsdevices_nfclocator_macaddress = '硬體位址';
    $vilsdevices_nfclocator_LoRAMacaddress = 'SMIMS LoRA 位址';
    $vilsdevices_nfclocator_gwid = 'LoRA Gateway ID';
    $vilsdevices_nfclocator_operation = '操作';
    $vilsdevices_nfclocator_creatnewdevice = '新增裝置';
    $vilsdevices_wifilocator_alive = '活動';
    $vilsdevices_wifilocator_name = '裝置名稱';
    $vilsdevices_wifilocator_type = '機型';
    $vilsdevices_wifilocator_macaddress = '硬體位址';
    $vilsdevices_wifilocator_apmac = 'AP macaddress';
    $vilsdevices_wifilocator_apssid = 'AP SSID';
    $vilsdevices_wifilocator_operation = '操作';
    $vilsdevices_edgecontol_alive = '活動';
    $vilsdevices_edgecontol_name = '裝置名稱';
    $vilsdevices_edgecontol_type = '機型';
    $vilsdevices_edgecontol_ip = 'IP位址';
    $vilsdevices_edgecontol_macaddress = '硬體位址';
    $vilsdevices_edgecontol_LoRAMacaddress = 'SMIMS LoRA 位址';
    $vilsdevices_edgecontol_gwid = 'LoRA Gateway ID';
    $vilsdevices_edgecontol_version = '版本';
    $vilsdevices_edgecontol_operation = '操作';
    $vilsdevices_uhfreader_name = '裝置名稱';
    $vilsdevices_uhfreader_type = '機型';
    $vilsdevices_uhfreader_ip = 'IP位址';

    $vilsrfiddevice_title = 'RFID裝置列表';
    $vilsrfiddevice_alive = '活動';
    $vilsrfiddevice_name = '裝置名稱';
    $vilsrfiddevice_type = '機型';
    $vilsrfiddevice_ip = 'IP位址';
    $vilsrfiddevice_macaddress = '硬體位址';
    $vilsrfiddevice_LoRAMacaddress = 'SMIMS LoRA 位址';
    $vilsrfiddevice_gwid = 'LoRA Gateway ID';
    $vilsrfiddevice_operation = '操作';
    $vilsrfiddevice_creatnewdevice = '新增裝置';

    $vilsnfccard_user = '目前使用者';
    $vilsnfccard_operation = '操作';

    $vilsrfidcard_title = 'RFID卡片列表';
    $vilsrfidcard_id = '卡片ID';
    $vilsrfidcard_user = '目前使用者';
    $vilsrfidcard_operation = '操作';
    $vilsrfidcard_creatnewcard = '新增卡片';
    $vilsnfccard_scannewcard = '掃描卡片';
    $vilsrfidcard_scannewcard = '掃描卡片';

    $vilsnfccard_unknown_alive = '活動';
    $vilsnfccard_unknown_id = '卡片ID';
    $vilsnfccard_unknown_nfcdevice = '感應裝置名稱';
    $vilsnfccard_unknown_updatetime = '感應時間';
    $vilsnfccard_unknown_operation = '操作';

    $vilsrfidcard_unknown_alive = '活動';
    $vilsrfidcard_unknown_id = '卡片ID';
    $vilsrfidcard_unknown_nfcdevice = '感應裝置名稱';
    $vilsrfidcard_unknown_updatetime = '感應時間';
    $vilsrfidcard_unknown_operation = '操作';

    $cardrollcall_title = '卡片點名排程';
    $cardrollcall_name = '排程名稱';
    $cardrollcall_timestart = '排程開始時間';
    $cardrollcall_timeend = '排程結束時間';
    $cardrollcall_date = '排程日期';
    $cardrollcall_device = '使用點名裝置';
    $cardrollcall_subgroups = '點名子集合';
    $cardrollcall_pushnotify = '排程推播設定';
    $cardrollcall_operation = '操作';
    $cardrollcall_creatnewrollcall = '新增排程';

    $cardrollcallevent_title = '卡片點名排程紀錄';
    $cardrollcallevent_name = '排程名稱';
    $cardrollcallevent_startdatetime = '點名開始時間';
    $cardrollcallevent_enddatetime = '點名結束時間';
    $cardrollcallevent_totalcount = '應到總數';
    $cardrollcallevent_presentcount = '實到總數';
    $cardrollcallevent_absentcount = '未到總數';
    $cardrollcallevent_operation = '操作';

    $cardrollcallresult_title = '卡片點名結果';
    $cardrollcallresult_name = '排程名稱';
    $cardrollcallresult_startdatetime = '點名開始時間';
    $cardrollcallresult_enddatetime = '點名結束時間';
    $cardrollcallresult_totalcount = '應到總數';
    $cardrollcallresult_presentcount = '實到總數';
    $cardrollcallresult_absentcount = '未到總數';
    $cardrollcallresult_cardid = '卡片ID';
    $cardrollcallresult_device = '點名裝置';
    $cardrollcallresult_user = '使用者';
    $cardrollcallresult_node = '標籤';
    $cardrollcallresult_present = '點名結果';
    $cardrollcallresult_time = '點名時間';
    $cardrollcallresult_operation = '操作';

    $userrollcallresult_title = '人員點名結果';
    $userrollcallresult_name = '排程名稱';
    $userrollcallresult_startdatetime = '點名開始時間';
    $userrollcallresult_enddatetime = '點名結束時間';
    $userrollcallresult_totalcount = '應到總數';
    $userrollcallresult_presentcount = '實到總數';
    $userrollcallresult_absentcount = '未到總數';
    $userrollcallresult_cardid = '卡片ID';
    $userrollcallresult_device = '點名裝置';
    $userrollcallresult_user = '使用者';
    $userrollcallresult_node = '標籤';
    $userrollcallresult_present = '點名結果';
    $userrollcallresult_time = '點名時間';
    $userrollcallresult_operation = '操作';

    $cardrollcallrecord_title = '卡片點名紀錄';
    $cardrollcallrecord_name = '排程名稱';
    $cardrollcallrecord_cardid = '卡片ID';
    $cardrollcallrecord_device = '點名裝置';
    $cardrollcallrecord_user = '使用者';
    $cardrollcallrecord_node = '標籤';
    $cardrollcallrecord_present = '點名結果';
    $cardrollcallrecord_time = '點名時間';
    $cardrollcallrecord_operation = '操作';

    $userrollcallrecord_title = '人員點名紀錄';
    $userrollcallrecord_name = '排程名稱';
    $userrollcallrecord_cardid = '卡片ID';
    $userrollcallrecord_device = '點名裝置';
    $userrollcallrecord_user = '使用者';
    $userrollcallrecord_node = '標籤';
    $userrollcallrecord_present = '點名結果';
    $userrollcallrecord_time = '點名時間';
    $userrollcallrecord_operation = '操作';

    $vilspushnotification_title = '推播訊息';
    $vilspushnotification_sendto = '推播訊息給';
    $vilspushnotification_tag = '標籤';
    $vilspushnotification_locator = 'Locator';
    $vilspushnotification_role = '角色';
    $vilspushnotification_user = '人員';
    $vilspushnotification_username = '人員名稱';
    $vilspushnotification_subgroup = '子集合';
    $vilspushnotification_searchresultname = '名稱';
    $vilspushnotification_records = '推播紀錄';
    $vilspushnotification_android_sapp_dl = '北瀚雲端通知 Android APP';
    $vilspushnotification_ios_app_dl = '北瀚雲端通知 iOS APP';

    $vilssurvey_title = '待辦事項';
    $vilssurvey_createnewsurvey = '新增待辦事項';

    $vilssurveyrecord_title = '待辦事項紀錄';

    $vilsassist_title = '協助事項';
    $vilsassist_createnew = '新增協助事項';

    $vilsassistrecord_title = '協助紀錄';
    $vilsassist_applynew = '申請協助';

    $vilsbillboard_title = '發新公告';
    $vilsbillboard_subgroup = '子集合';
    $vilsbillboard_sendto = '發公告給';
    $vilsbillboardrecords_title = '公告事項';

    $vilsuserinfo_title = '人員列表';
    $vilsuserinfo_name = '名稱';
    $vilsuserinfo_account = '帳號';
    $vilsuserinfo_id = '識別碼';
    $vilsuserinfo_unit = '單位';
    $vilsuserinfo_usertitle = '職稱';
    $vilsuserinfo_gender = '性別';
    $vilsuserinfo_photo = '照片';
    $vilsuserinfo_group = '功能權限群組';
    $vilsuserinfo_datagroup = '資料權限群組';
    $vilsuserinfo_subgroup = '子集合';
    $vilsuserinfo_rfidcards = 'RFID卡片';
    $vilsuserinfo_nfccards = 'NFC卡片';
    $vilsuserinfo_uwbcards = 'UWB卡片';
    $vilsuserinfo_creatnewuser = '新增人員';
    $vilsuserinfo_operation = '操作';

    $vilsusergroupinfo_title = '群組列表';
    $vilsusergroupinfo_groupname = '名稱';
    $vilsusergroupinfo_permission = '權限';
    $vilsusergroupinfo_operation = '操作';
    $vilsusergroupinfo_creatnewusergroup = '新增群組';

    $vilsusermaingroupinfo_title = '主集合';
    $vilsusermaingroupinfo_groupname = '名稱';
    $vilsusermaingroupinfo_permission = '說明';
    $vilsusermaingroupinfo_manager = '主集合管理者';
    $vilsusermaingroupinfo_operation = '操作';

    $vilsusersubgroupinfo_title = '子集合';
    $vilsusersubgroupinfo_groupname = '名稱';
    $vilsusersubgroupinfo_permission = '說明';
    $vilsusersubgroupinfo_photo = '照片';
    $vilsusersubgroupinfo_manager = '子集合管理者';
    $vilsusersubgroupinfo_operation = '操作';
    $vilsusersubgroupinfo_creatnewusergroup = '新增子集合';

    $vilscardscannfcresult_title = 'NFC卡片感應記錄';
    $vilscardscannfcresult_loading = '載入中...';
    $vilscardscannfcresult_cardname = '卡片名稱';
    $vilscardscannfcresult_cardid = '卡片ID';
    $vilscardscannfcresult_nfcdevice = '感應裝置名稱';
    $vilscardscannfcresult_user = '人員名稱';
    $vilscardscannfcresult_updatetime = '感應時間';
    $vilscardscannfcresult_ssid = '使用 SSID';
    $vilscardscannfcresult_wifimac = 'AP mac';

    $vilscardscanuhfresult_title = 'UHF卡片感應記錄';
    $vilscardscanuhfresult_loading = '載入中...';
    $vilscardscanuhfresult_cardname = '卡片名稱';
    $vilscardscanuhfresult_cardid = '卡片ID';
    $vilscardscanuhfresult_nfcdevice = '感應裝置名稱';
    $vilscardscanuhfresult_user = '人員名稱';
    $vilscardscanuhfresult_updatetime = '感應時間';

    $vilscardscanuhfresult_title = 'UHF卡片感應記錄';
    $vilscardscanuhfresult_loading = '載入中...';

    $vilsbodyinfo_title = '生理量測';
    $vilsbodyinfo_abbox = '訊號採集器';

    $vilslocation_title = '標籤所在區域';
    $vilslocation_location = '區域';
    $vilslocation_type = '型態';
    $vilslocation_activity = '活動';
    $vilslocation_device = '裝置序號/名稱';

    $vilsranging_title = '測距記錄';
    $vilsranging_loading = '載入中...';

    $vils2dmap_title = '室內地圖';
    $vils2dmap_edit_area_name = '編輯區域名稱';
    $vils2dmap_edit_area_type_0 = '普通';
    $vils2dmap_edit_area_type_1 = '電子圍欄';
    $vils2dmap_edit_area_type_2 = '電子點名';
    $vils2dmap_edit_area_type_3 = '房間';
    $vils2dmap_edit_area_type_4 = '教室';
    $vils2dmap_edit_area_type_5 = '病房';
    $vils2dmap_edit_area_type_6 = '病床';
    $vils2dmap_edit_area_type_999 = '虛擬牆';
    $vils2dmap_edit_save = '儲存';
    $vils2dmap_general = '一般設定';
    $vils2dmap_changemap = '更換地圖';
    $vils2dmap_show_locators = '顯示固定裝置';
    $vils2dmap_show_tags = '顯示人員裝置';

    $vilsactivity_title = '物聯網';
    $vilsactivity_tag = '標籤';
    $vilsactivity_anchor = '基站';
    $vilsactivity_coordinator = '閘道';
    $vilsactivity_locator = 'Locator';
    $vilsactivity_typeindex = '形式/編號';
    $vilsactivity_macaddress = '硬體位址';
    $vilsactivity_timestamp = '時間';
    $vilsactivity_activity = '活動';

    $vilsmaintenance_title = '裝置履歷';
    $vilsmaintenance_nodetype = '裝置形式';
    $vilsmaintenance_nodeid = '裝置序號';
    $vilsmaintenance_name = '儀器名稱';
    $vilsmaintenance_type = '廠牌型別';
    $vilsmaintenance_unit = '保管單位';
    $vilsmaintenance_manager = '管理人員';
    $vilsmaintenance_assetlife = '資產年限';
    $vilsmaintenance_maintenancedate = '保養校正(日期通知)';
    $vilsmaintenance_status = '儀器設備狀況(良好、維修校正、故障)';
    $vilsmaintenance_datetime = '紀錄時間';
    $vilsmaintenance_createnewrecord = '新增紀錄';

    $vilsfootprint_title = '足跡記錄';
    $vilsfootprint_select_tag = '選擇標籤';
    $vilsfootprint_pause = '暫停';
    $vilsfootprint_play = '播放';
    $vilsfootprint_speed_1 = '1倍速度';
    $vilsfootprint_speed_4 = '4倍速度';
    $vilsfootprint_speed_16 = '16倍速度';
    $vilsfootprint_speed_60 = '60倍速度';

    $vilsfootprint_courses_list = '課程列表';
    $vilsfootprint_courses_name = '課程名稱';
    $vilsfootprint_courses_description = '課程描述';
    $vilsfootprint_courses_datetime = '日期時間';
    $vilsfootprint_areas_name = '區域名稱';
    $vilsfootprint_areas_type = '區域型態';

    $vilsfootprint_reload = '繼續載入';
    $vilsfootprint_nodata = '已無資料';

    $vilsevent_title = '標籤觸發事件';

    $vilssection_title = '區段設定';
    $vilssection_Anchor = '使用基站';
    $vilssection_Anchor_1 = '做判斷, ';
    $vilssection_At = '所在區域';
    $vilssection_Range = '範圍內(公分):';
    $vilssection_start_at = '座標開始點(X,Y,Z)';
    $vilssection_to_end = '到結束點(X,Y,Z)';
    $vilssection_create = '新增設定';

    $vilsfencealert_title = '電子圍籬';
    $vilsfencealert_Anchor = '使用基站';
    $vilsfencealert_Anchor_1 = '做判斷, ';
    $vilsfencealert_At = '所在區域';
    $vilsfencealert_Range = '範圍內(公分):';
    $vilsfencealert_create = '新增設定';

    $filestorage_creatnewfile = '新增檔案';

    $filestorage_creatnewimage = '新增相片';

    $applicationform_leave_createnew = '新增請假單';
    $applicationform_leave_filter_nonesignoff = '只顯示尚未簽核';
    $applicationform_leave_serialnum = '表單編號';
    $applicationform_leave_applytime = '申請時間';
    $applicationform_leave_user = '請假人員';
    $applicationform_leave_type = '假勤項目';
    $applicationform_leave_reason = '事由';
    $applicationform_leave_timerange = '請假時間';
    $applicationform_leave_reissue = '補請假';
    $applicationform_leave_photo = '附件照片';
    $applicationform_leave_file = '附件檔案';
    $applicationform_leave_applyusername = '申請人姓名';
    $applicationform_leave_applyuserrelation = '申請人關係';
    $applicationform_leave_status = '簽核流程';
    $applicationform_leave_signoff = '簽核人員';

    $vilssetting_title = '專案設定';
    $vilssetting_building = '樓號';
    $vilssetting_floor = '樓層';
    $vilssetting_restart = '重新啟動';
    $vilssetting_save = '儲存';

    $vilsdisetting_title = 'DI卡片項目設定';
    $vilsdisetting_subgroup = '子集合';

    $vilspushnotify_title = '推播項目設定';
    $vilspushnotify_target = '設定對象';
    $vilspushnotify_employees = '工作人員';
    $vilspushnotify_setting = '推播設定';
    $vilspushnotify_search = '搜尋';

    // $vilscheckpointpath_title = '巡檢路線';
    // $vilscheckpointpath_setting_title = '巡檢路線設定';
    // $vilscheckpointpath_record_title = '巡檢路線記錄';
    // $vilscheckpointpath_loading = '載入中...';
    // $vilscheckpointrecord_loading  = '载入中...';

    // $vilscheckpointhistory_title = '巡檢路線歷史記錄';

    $vilsinspectionpath_title = '巡檢路線';
    $vilsinspectionpath_setting_title = '巡檢路線設定';
    $vilsinspectionpath_record_title = '巡檢路線記錄';
    $vilsinspectionpath_loading = '載入中...';
    $vilsinspectionrecord_loading  = '載入中...';

    $vilsinspectionhistory_title = '巡檢路線歷史記錄';

    $vilsinspection_report_title = '巡檢報告';
    $vilsinspection_daily_report = '日報表';
    $vilsinspection_weekly_report = '週報表';
    $vilsinspection_monthly_report = '月報表';
    $vilsinspection_generate_report = '產生報表';

    $vilsalerteventtimeline_alive = '活動';
    $vilsalerteventtimeline_floor = '樓層';
    $vilsalerteventtimeline_timeline = '事件時間軸';

    $vilspowermeterview_pm_tag = '智慧電錶';
    $vilspowermeterview_pm_id = '智慧錶表ID';

    $vilslightdevicecontrol_light_tag = '智慧光源(單)';
    $vilslightdevicecontrol_lightGroup_tag = '智慧光源(群)';
    $vilslightdevicecontrol_light_id = '智慧光源ID';
    $vilslightdevicecontrol_lg_id = '群組名稱';
?>
