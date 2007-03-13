class SE_Renderer;
class SE_LineBuffer;
struct SE_Matrix;
struct SE_Style;
struct SE_RenderStyle;


class SE_PositioningAlgorithms
{

public:

    static void EightSurrounding(SE_Renderer*    renderer, 
                          LineBuffer*     geometry, 
                          SE_Matrix&      xform, 
                          SE_Style*       style, 
                          SE_RenderStyle* rstyle, 
                          double          mm2px
                          );

    static void PathLabels(SE_Renderer*    renderer, 
                          LineBuffer*     geometry, 
                          SE_Matrix&      xform, 
                          SE_Style*       style, 
                          SE_RenderStyle* rstyle, 
                          double          mm2px
                          );

    static void MultipleHighwaysShields(SE_Renderer*    renderer, 
                                        LineBuffer*     geometry, 
                                        SE_Matrix&      xform, 
                                        SE_Style*       style, 
                                        SE_RenderStyle* rstyle, 
                                        double          mm2px
                                        );

private:







};
