/* ===================================================================== *
 * ---------------------- PROGRAMA DE CAPTURA -------------------------  *
 * ===================================================================== *
 *  Multiplas cameras usando libdc1394                                   *
 *  Autor: Oswaldo Bassani Neto                                          *
 *  Baseado em: Samuel Perfidio                                          *
 *-----------------------------------------------------------------------*/

void setDebugLevel(int level);

/* ========================== vendor() ================================= */
void vendor(void);

/* ========================== model() ================================== */
void model(void);

/* ========================== setCaptureFormat() ======================= */
void setCaptureFormat(void);

/* ========================== configure() ============================== */
int configure(void);

/* ========================== start() ================================== */
int start(void);

/* ========================== getFrame() =============================== */
int getFrame(void);

unsigned char* getRgbBuffer_left();
unsigned char* getRgbBuffer_right();

unsigned char* getRgbBuffer_left_right();

/* ========================== stop() =================================== */
void stop(void);

/* ========================== cleanup() ================================ *
 *  called when the program exits and                                    *
 *  destroys the existing connections to the 1394 drivers                *
 *-----------------------------------------------------------------------*/
void cleanup(void);

