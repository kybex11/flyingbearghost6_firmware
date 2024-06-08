#ifndef GCode_H_
#define GCode_H_

#include <string>
#include <vector>
#include <limits>
#include <QVector3D>
#include <QVector>
#include <cmath>

using namespace std;

struct GCodeParameter
{
	char name;
	float value;
};

struct GCodeLine
{
	string originalLine; 
	string clearedLine;  
	string command;
    bool interprete;
    bool hasE;
    bool hasXYZ;
    bool visible;
    int layer;

    float x;
    float y;
    float z;
    float e;
    float f;
};

class GCode
{
public:
    GCode();
    ~GCode();
    int   open(string fileName);
    void  clear();
    void  draw(bool linesOnly, bool showMotion);
    int   addLine(string line);
    void  parseLine(GCodeLine &gcodeLine, char command, string value);
    vector<GCodeLine>& getCodeLines() ;
    void  refreshMinMax();
	float getMinX();
	float getMinY();
	float getMinZ();
	float getMaxX();
	float getMaxY();
	float getMaxZ();
    int  getGCodeCount() {
        return codeLines.size();
    }
    void setGCodeLayers(int layers);

    bool  isOpen() const;
protected:
	
private:
	vector<GCodeLine> codeLines;
	void replace(string& replace, string from, string to);
	void explode(vector<string> &res, string &list, string expression);
    void generateTube(QVector3D &p1, QVector3D &p2, QVector3D &p3, bool saveRearFacet, float radius);
    void recomputeAll();

	float minX, minY, minZ;
	float maxX, maxY, maxZ;
	float lastZ;
	int currentLayer;
    int showLayers;

    QVector<QVector3D> m_tubeVertices;
    QVector<int> m_tubeIndices;
    QVector<QVector3D> m_tubeNormals;
    QVector<QVector3D> m_prevFacet;

};

#endif /* GCode_H_ */