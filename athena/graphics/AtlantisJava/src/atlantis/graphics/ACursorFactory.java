package atlantis.graphics;


import java.awt.Toolkit;
import java.awt.Dimension;
import java.awt.Cursor;
import java.awt.Point;
import java.awt.Image;
import java.awt.dnd.DragSource;

import atlantis.globals.AGlobals;
import atlantis.utils.ALogger;
import atlantis.utils.AUtilities;

/**
 * Singleton class factory for producing cursors. 
 * Due to difference in the default cursors provided by the different OSs,
 * all cursors are generated by Atlantis itself.
 */

public class ACursorFactory {
    
    private static ALogger logger = ALogger.getLogger(ACursorFactory.class);
    
    /** singleton class, pointer to this instance */
    private static ACursorFactory instance;
    
    /** Cursor size string, either "64x64" or "32x32" */
    private String cursorSize;
    
    /** Cursor color string, either "bw" or "" */
    private String bAndW="";
    
    /**
     * @return true if if system supports custom cursors
     */
    public boolean getCustomCursors() { return customCursors; }
    private final boolean customCursors;

    /** 
     * @return the default Cursor 
     */
    public Cursor getDefaultCursor() { return defaultCursor; }
    private Cursor defaultCursor = null;
   
    /** Drag-and-drop valid cursor */
    private Cursor dragValidCursor=null;
    public Cursor getDragValidCursor() { return dragValidCursor; }
    /** Drag-and-drop invalid cursor */
    private Cursor dragInvalidCursor=null;
    public Cursor getDragInvalidCursor() { return dragValidCursor; }
    
    /** Zoom cursor */
    public Cursor getZoomCursor() { return zoomCursor; }
    private Cursor zoomCursor=null;
    /** Move cursor */
    public Cursor getMoveCursor() { return moveCursor; }
    private Cursor moveCursor=null;
    /** Rotate cursor */
    public Cursor getRotateCursor() { return rotateCursor; }
    private Cursor rotateCursor=null;

    /** Pick cursor */
    public Cursor getPickCursor() { return pickCursor; }
    private Cursor pickCursor=null;
    /** Rectangular selection cursor */
    public Cursor getRectSelectCursor() { return rectSelectCursor; }
    private Cursor rectSelectCursor=null;

    /** Syncro cursor */
    public Cursor getSyncCursor() { return syncCursor; }
    private Cursor syncCursor=null;
    /** FishEye cursor */
    public Cursor getFishEyeCursor() { return fishEyeCursor; }
    private Cursor fishEyeCursor=null;
    
    /** FishEye symbol also needed as a FishEye on/off indicator */
    public Image getFishEyeIndicator() { return fishEyeIndicator; }
    private Image fishEyeIndicator=null;

    /** ATLAS logo can be printed on canvas */
    public Image getAtlasLogo() { return atlasLogo; }
    private Image atlasLogo=null;

    /**
     * Public singleton instantiation accessor
     * @return the singleton instance
     */
    public static ACursorFactory getInstance() {

      //Check if we already have an instance
      if (instance == null) instance = new ACursorFactory();

      //Return the singleton instance
      return instance;
    }


    /** Constructor */
    private ACursorFactory(){
              
      logger.debug("Creating cursor and image factory ...");
      String FILE_SEPAR = System.getProperty("file.separator");
      String homeDirectory = AGlobals.instance().getHomeDirectory();
      
      //Provide images and indicators even in headless mode
      //Fisheye
      String path = homeDirectory + "img" + FILE_SEPAR +
             "cursor_fisheye_"+bAndW+"32x32.gif";
      fishEyeIndicator = AUtilities.getFileAsImageIcon(path).getImage();

      //Atlas logo
      path = homeDirectory + "img" + FILE_SEPAR +
             "atlas_logo_big.png";
      atlasLogo = AUtilities.getFileAsImageIcon(path).getImage();


      //Don't create any cursors in headless mode
      if (AGlobals.isAtlantisHeadless()){
        //Custom cursors not supported on this platform
        customCursors=false;
        return;
      }

      //Check for best cursor size on this system - trying largest
      Dimension bestCursorSize=Toolkit.getDefaultToolkit().getBestCursorSize(64,64);
      
      //Check if custom cursors are allowed
      if (bestCursorSize.width > 0){
          
        customCursors=true;
      
        //We only support 32x32 and 64x64 cursors. Check which one we are closer to
        if (bestCursorSize.width < 48) cursorSize="32x32";
        else cursorSize="64x64";
        
        //Check if we have enough colours for greyscale cursors
        if (Toolkit.getDefaultToolkit().getMaximumCursorColors() < 255)
          bAndW="bw_";
        else bAndW="";
          
        logger.info("Using "+bAndW+cursorSize+" cursors");

        //Now create the individual cursors
        Toolkit tools = Toolkit.getDefaultToolkit();
        
        
        //Drag and drop cursors -----------------------------------------------
        
        //Valid DnD
        path = homeDirectory + "img" + FILE_SEPAR + 
                      "cursor_DnD_valid_" + cursorSize+ ".gif";
        Image validDnDImg = AUtilities.getFileAsImageIcon(path).getImage();
        
        dragValidCursor=tools.createCustomCursor(validDnDImg, new Point(1, 1),"ValidDnD");        
        
        //Invalid Dnd
        path = homeDirectory + "img" + FILE_SEPAR +
               "cursor_DnD_invalid_" + cursorSize + ".gif";
        Image invalidDnDImg = AUtilities.getFileAsImageIcon(path).getImage();
        dragInvalidCursor=tools.createCustomCursor(invalidDnDImg,new Point(9, 9),"InvalidDnD");
        
        //Zoom, move and rotate cursors -----------------------------------------------
        //Zoom
        path = homeDirectory + "img" + FILE_SEPAR + 
               "cursor_zoom_"+bAndW+cursorSize+".gif";
        Image zoomImg = AUtilities.getFileAsImageIcon(path).getImage();
        zoomCursor=tools.createCustomCursor(zoomImg, new Point(9, 9),"Zoom");
                
        //Move
        path = homeDirectory + "img" + FILE_SEPAR + 
               "cursor_move_" + cursorSize+".gif";
        Image moveImg = AUtilities.getFileAsImageIcon(path).getImage();
        moveCursor=tools.createCustomCursor(moveImg,new Point(10, 10),"Move");
                
        //Rotate
        path = homeDirectory + "img" + FILE_SEPAR + 
               "cursor_rotate_"+bAndW+cursorSize+".gif";
        Image rotateImg = AUtilities.getFileAsImageIcon(path).getImage();
        rotateCursor=tools.createCustomCursor(rotateImg,new Point(11, 11),"Rotate");
        
        //Pick and RectSelect cursors -----------------------------------------------
        //Pick
        path = homeDirectory + "img" + FILE_SEPAR +
               "cursor_pick_"+cursorSize+".gif";
        Image pickImg = AUtilities.getFileAsImageIcon(path).getImage();
        pickCursor=tools.createCustomCursor(pickImg, new Point(6, 1),"Pick");
                
        //Rectangle select
        path = homeDirectory + "img" + FILE_SEPAR +
               "cursor_rectsel_"+cursorSize+".gif";
        Image rectSelectImg = AUtilities.getFileAsImageIcon(path).getImage();
        rectSelectCursor=tools.createCustomCursor(rectSelectImg,new Point(12, 12),"RectSelect");
        
        //Syncro and FishEye cursors -----------------------------------------------------
        //Sync
        path = homeDirectory + "img" + FILE_SEPAR + 
               "cursor_sync_" + cursorSize + ".gif";
        Image syncImg = AUtilities.getFileAsImageIcon(path).getImage();
        syncCursor=tools.createCustomCursor(syncImg, new Point(10, 10),"Sync");
        
        //FishEye 
        path = homeDirectory + "img" + FILE_SEPAR + 
               "cursor_fisheye_"+bAndW+cursorSize+".gif";
        Image fishEyeImg = AUtilities.getFileAsImageIcon(path).getImage();
        fishEyeCursor=tools.createCustomCursor(fishEyeImg, new Point(1, 8),"FishEye");
       
      } else customCursors=false;
      
          
      //Now check for any cursors that have not been set yet
      //And set them to system defaults      
      if (defaultCursor == null) defaultCursor = new Cursor(Cursor.DEFAULT_CURSOR);
      
      if (dragValidCursor == null) dragValidCursor = DragSource.DefaultMoveDrop;
      if (dragInvalidCursor == null) dragInvalidCursor = DragSource.DefaultMoveNoDrop;

      if (zoomCursor == null) zoomCursor = new Cursor(Cursor.DEFAULT_CURSOR);
      if (moveCursor == null) moveCursor = new Cursor(Cursor.MOVE_CURSOR);
      if (rotateCursor == null) rotateCursor  = new Cursor(Cursor.DEFAULT_CURSOR);
 
      if (pickCursor == null) pickCursor = new Cursor(Cursor.HAND_CURSOR);
      if (rectSelectCursor == null) rectSelectCursor = new Cursor(Cursor.NW_RESIZE_CURSOR);
      if (syncCursor == null) syncCursor = new Cursor(Cursor.CROSSHAIR_CURSOR);
    }
     
}
