/*
* This file is part of Wakanda software, licensed by 4D under
*  (i) the GNU General Public License version 3 (GNU GPL v3), or
*  (ii) the Affero General Public License version 3 (AGPL v3) or
*  (iii) a commercial license.
* This file remains the exclusive property of 4D and/or its licensors
* and is protected by national and international legislations.
* In any event, Licensee's compliance with the terms and conditions
* of the applicable license constitutes a prerequisite to any use of this file.
* Except as otherwise expressly stated in the applicable license,
* such license does not include any other license or rights on this file,
* 4D's and/or its licensors' trademarks and/or other proprietary rights.
* Consequently, no title, copyright or other proprietary rights
* other than those specified in the applicable license is granted.
*/
    /**
    * set the split zone & spliters size
    * @param {bool} init true if called the first time in order to init the view
    * @param {string} target 
    */
    setSpliter = function( init, target ) {
     
        var windowWidth,
            leftWidth,
            halfSpliter,
            halfSpliterRight,
            rightHeight,
            topHeight,
            rightLayoutWidth;
     
    

            rightLayoutWidth = _conf.admin_rightLayoutWidth;
       
        
       
               $('#administration-workspace').split({ orientation: 'vertical', limit: rightLayoutWidth });
        

       //set layout size
       windowWidth  = window.innerWidth;
       leftWidth    = windowWidth - rightLayoutWidth;       
       $("#administration-workspace-left").css("width", leftWidth - _conf.spliterWidth);
       $(".vspliter").css("left", leftWidth - _conf.spliterWidth);
       $("#administration-workspace-right").css("width", rightLayoutWidth +"px");

      
//       rightHeight = 174;
//       topHeight = rightHeight - _conf.bottomLayoutHeight;
//       $("#administration-workspace-right-top").css("height", topHeight- _conf.spliterWidth);
//       $(".hspliter").css("top", topHeight - _conf.spliterWidth);
//       $("#administration-workspace-right-bottom").css("height", _conf.bottomLayoutHeight);
   
   
        //si pas init, conserver la taille actuel de bottom et de left
       
    }
    
     
  
    