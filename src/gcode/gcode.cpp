#include "gcode.h"
#include <fstream>
#include <iostream>
#include <QString>
#include <QtOpenGL>

#define gPushTriangleToList(v1, v2, v3) m_tubeVertices.push_back(v1);\
                                     m_tubeVertices.push_back(v2);\
                                     m_tubeVertices.push_back(v3);

#define VectorOutput(m, v) qDebug() << QString(QString(m) + "x(%1), y(%2), z(%3)").arg(v.x()).arg(v.y()).arg(v.z());

GCode::GCode()
{
	
}

GCode::~GCode()
{
	
}

void GCode::draw(bool linesOnly, bool showMotion)
{
    if (linesOnly) {
        glPushMatrix();
        float oldx = 0, oldy = 0, oldz = 0;
        glBegin(GL_LINES);
        float minusX = maxX * 0.5;
        float minusY = maxY * 0.5;
        for(unsigned int i = 0; i < showLayers; i++) {

            if(codeLines[i].command == "G1") { 

                if(codeLines[i].hasE && codeLines[i].hasXYZ) {
                    glColor3f(0.11, 0.15, 0.5);
                    glVertex3f(oldx, oldz, oldy);
                    glVertex3f((codeLines[i].x-minusX), codeLines[i].z, codeLines[i].y-minusY);
                    oldx = codeLines[i].x - minusX;
                    oldy = codeLines[i].y - minusY;
                    oldz = codeLines[i].z;
                } else if (codeLines[i].hasXYZ) {
                    if (showMotion) {
                        glColor3f(0.91, 0.24, 0.1);
                        glVertex3f(oldx, oldz, oldy);
                        glVertex3f(codeLines[i].x-minusX, codeLines[i].z, codeLines[i].y-minusY);
                    }
                    oldx = codeLines[i].x - minusX;
                    oldy = codeLines[i].y - minusY;
                    oldz = codeLines[i].z;
                }
            }
        }

        glEnd();
        glPopMatrix();
    } else {
        glEnable(GL_COLOR_MATERIAL);
        glEnable(GL_LIGHT0);
        glEnable(GL_LIGHTING);

        glPushMatrix();
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);

        glVertexPointer(3, GL_FLOAT, 0, (float *)m_tubeVertices.data());
        glNormalPointer(GL_FLOAT, 0, (float *)m_tubeNormals.data());
        glDrawElements(GL_TRIANGLES, m_tubeIndices.size(), GL_UNSIGNED_INT, m_tubeIndices.data());

        glDisableClientState(GL_NORMAL_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);

        glPopMatrix();

        glDisable(GL_COLOR_MATERIAL);
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHTING);
    }
}

int GCode::open(string fileName)
{
    currentLayer = 0;
    lastZ = 0;
    minX =  1000000.0;
    minY =  1000000.0;
    minZ =  1000000.0;
    maxX = -1000000.0;
    maxY = -1000000.0;
    maxZ = -1000000.0;
    ifstream inFile(fileName.c_str(), ios::binary);
    while(!inFile.eof())
    {
        char lineBuffer[1000];
        inFile.getline(lineBuffer, 1000);
        string line = lineBuffer;
        addLine(line);
    }
    recomputeAll();

	return 0;
}

void GCode::clear()
{
	codeLines.clear();
}

int GCode::addLine(string line)
{
	GCodeLine codeLine;
    codeLine.originalLine = line;
    string clearedLine = line;
	size_t pos;
    if((pos = clearedLine.find(";")) != string::npos)
        clearedLine = clearedLine.substr(0, pos);
	
    while(clearedLine.length() && (clearedLine[0] == ' ' || clearedLine[0] == '\t'))
        clearedLine = clearedLine.substr(1, clearedLine.length()-1);
	
    while(clearedLine.length() && (clearedLine[clearedLine.length()-1] == ' ' || clearedLine[clearedLine.length()-1] == '\t'))
        clearedLine = clearedLine.substr(0, clearedLine.length()-1);
	
	replace(clearedLine, "\t", " ");
	
    while(clearedLine.find("  ") != string::npos)
		replace(clearedLine, "  ", " ");

    qDebug() << QString::fromUtf8(clearedLine.c_str());
    if(clearedLine.length() <= 1) {
        codeLine.interprete = false;
        codeLine.command = "";
    } else {
        codeLine.interprete = true;

		vector<string> words;
		explode(words, clearedLine, " ");

        if(words.size() != 0) {
            codeLine.hasE = codeLine.hasXYZ = false;
            codeLine.command = words[0];
            codeLine.x = codeLine.y = codeLine.z = codeLine.e = codeLine.f = 0.f;

            for(unsigned int i = 1; i < words.size(); ++i) {

                if(words[i].length() == 0)
					continue;

                string strValue = words[i].substr(1, words[i].length()-1);
                parseLine(codeLine, words[i][0], strValue);
			}

            if(codeLine.command == "G1" && codeLine.hasE && codeLine.hasXYZ) {
                if(codeLine.x < minX)
                    minX = codeLine.x;
                if(codeLine.x > maxX)
                    maxX = codeLine.x;
                if(codeLine.y < minY)
                    minY = codeLine.y;
                if(codeLine.y > maxY)
                    maxY = codeLine.y;
                if(codeLine.z < minZ)
                    minZ = codeLine.z;
                if(codeLine.z > maxZ)
                    maxZ = codeLine.z;
            }

            codeLine.layer = currentLayer;
		}
	}

    codeLine.clearedLine = clearedLine;
    if(codeLine.command == "G1" && codeLine.hasXYZ) {
        codeLines.push_back(codeLine);
    }

	return 0;
}

void GCode::parseLine(GCodeLine &gcodeLine, char command, string value)
{
    float fValue = atof(value.c_str());
    switch(command) {
    case 'X':
        gcodeLine.hasXYZ = true;
        gcodeLine.x = fValue;
        gcodeLine.z = lastZ;
        break;
    case 'Y':
        gcodeLine.hasXYZ = true;
        gcodeLine.y = fValue;
        gcodeLine.z = lastZ;
        break;
    case 'Z':

        if (fValue != lastZ) {
            lastZ = fValue;
            currentLayer++;
        }
        break;
    case 'E':
        gcodeLine.hasE = true;
        gcodeLine.e = fValue;
        break;
    case 'F':
        gcodeLine.f = fValue;
        break;
    }
}

void GCode::refreshMinMax()
{
    qDebug() << "============== iterator ===============";
    float oldx = 0, oldy = 0, oldz = 0;    
    float minusX = maxX * 0.5;
    float minusY = maxY * 0.5;
    for(unsigned int i = 0; i < codeLines.size(); i++) {

        if(codeLines[i].command == "G1") { 
            qDebug() << QString::fromUtf8(codeLines[i].clearedLine.c_str());
            if(codeLines[i].hasE && codeLines[i].hasXYZ) {
                qDebug() << QString("[1] oldx(%1), oldy(%2), oldz(%3)").arg(oldx).arg(oldy).arg(oldz);
                qDebug() << QString("[1] x(%1), y(%2), z(%3)").arg(codeLines[i].x-minusX).arg(codeLines[i].y-minusY).arg(codeLines[i].z);
                oldx = codeLines[i].x - minusX;
                oldy = codeLines[i].y - minusY;
                oldz = codeLines[i].z;

            } else if(codeLines[i].hasXYZ) {

                oldx = codeLines[i].x - minusX;
                oldy = codeLines[i].y - minusY;
                oldz = codeLines[i].z;
                qDebug() << QString("[2] x(%1), y(%2), z(%3)").arg(oldx).arg(oldy).arg(oldz);
            }
        }
    }
    qDebug() << "============== end ===============";
}

void GCode::replace(string& replace, string from, string to)
{
	size_t pos;
    while((pos = replace.find(from)) != string::npos)
		replace.replace(pos, from.length(), to);
}

void GCode::explode(vector<string> &res, string &list, string expression)
{
    if(list.length() < 1)
		return;

    size_t posStart = 0, posEnd;
	while(1)
	{
        posEnd = list.find(expression, posStart);
        if(posEnd == string::npos)
			break;
        string line = list.substr(posStart, posEnd - posStart);

        if(line.length() > 0)
			res.push_back(line);
        posStart = posEnd + expression.length();
	}
    string line = list.substr(posStart, posEnd-posStart);
    if(line.length() > 0)
		res.push_back(line);
}

vector<GCodeLine>& GCode::getCodeLines()
{
	return codeLines;
}

float GCode::getMinX() { if(minX != minX) return 0; else return minX; }
float GCode::getMinY() { if(minY != minY) return 0; else return minY; }
float GCode::getMinZ() { if(minZ != minZ) return 0; else return minZ; }
float GCode::getMaxX() { if(maxX != maxX) return 0; else return maxX; }
float GCode::getMaxY() { if(maxY != maxY) return 0; else return maxY; }
float GCode::getMaxZ() { if(maxZ != maxZ) return 0; else return maxZ; }

bool GCode::isOpen() const
{
    return (codeLines.size() > 0);
}

void GCode::generateTube(QVector3D &p1, QVector3D &p2, QVector3D &p3, bool saveRearFacet = false, float radius = 0.27)
{
    qDebug() << "#####################";
    VectorOutput("==> ", p1);
    VectorOutput("==> ", p2);
    VectorOutput("==> ", p3);
    qDebug() << "#####################";

    float ratio = radius / sqrt( pow(p1.z() - p2.z(), 2) + pow(p2.x() - p1.x(), 2));
    QVector3D p1p2VertVector((p1.z() - p2.z()) * ratio, p1.y(), (p2.x() - p1.x()) * ratio);
    float shortRadius = (radius * 0.75);

    if (saveRearFacet) {
        qDebug() << "--------↓↓↓ saveRearFacet ↓↓↓-------";
        QVector3D s1(p2.x() + p1p2VertVector.x(), p2.y(), p2.z() + p1p2VertVector.z());
        QVector3D s2(p2.x() - p1p2VertVector.x(), p2.y(), p2.z() - p1p2VertVector.z());
        QVector3D p3Vert1(p2.x(), p2.y() + shortRadius, p2.z());
        QVector3D p3Vert2(p2.x(), p2.y() - shortRadius, p2.z());

        VectorOutput("[1-p1]", s1);
        VectorOutput("[1-p2]", p3Vert1);
        VectorOutput("[1-p3]", s2);
        gPushTriangleToList(s1, p3Vert1, s2);

        VectorOutput("[2-p1]", s1);
        VectorOutput("[2-p2]", s2);
        VectorOutput("[2-p3]", p3Vert2);
        gPushTriangleToList(s1, s2, p3Vert2);

        qDebug() << "m_prevFacet.size => " << m_prevFacet.size();
        if (m_prevFacet.size() == 0) {
            QVector3D f1(p1.x() + p1p2VertVector.x(), p1.y(), p1.z() + p1p2VertVector.z());
            QVector3D f2(p1.x() - p1p2VertVector.x(), p1.y(), p1.z() - p1p2VertVector.z());

            QVector3D p1Vert1(p1.x(), p1.y() + shortRadius, p1.z());
            QVector3D p1Vert2(p1.x(), p1.y() - shortRadius, p1.z());
            gPushTriangleToList(f1, s1     , p3Vert1);
            gPushTriangleToList(f1, p3Vert1, p1Vert1);
            gPushTriangleToList(f2, s2     , p1Vert1);
            gPushTriangleToList(s2, p3Vert1, p1Vert1);
            gPushTriangleToList(f1, p3Vert2, s1);
            gPushTriangleToList(f1, p1Vert2, p3Vert2);
            gPushTriangleToList(f2, s2     , p3Vert2);
            gPushTriangleToList(f2, p3Vert2, p1Vert2);

        } else {
            gPushTriangleToList(m_prevFacet.at(0), s1               , p3Vert1);
            gPushTriangleToList(m_prevFacet.at(0), p3Vert1          , m_prevFacet.at(2));
            gPushTriangleToList(m_prevFacet.at(1), s2               , m_prevFacet.at(2));
            gPushTriangleToList(s2               , p3Vert1          , m_prevFacet.at(2));
            gPushTriangleToList(m_prevFacet.at(0), p3Vert2          , s1);
            gPushTriangleToList(m_prevFacet.at(0), m_prevFacet.at(3), p3Vert2);
            gPushTriangleToList(m_prevFacet.at(1), s2               , p3Vert2);
            gPushTriangleToList(m_prevFacet.at(1), p3Vert2          , m_prevFacet.at(3));
        }
        qDebug() << "--------↑↑↑ saveRearFacet ↑↑↑-------";
    } else {
        qDebug() << "--------↓↓↓ calc half-angle vector ↓↓↓-------";
        QVector3D d1Vector = p1 - p2;
        QVector3D d2Vector = p3 - p2;

        float cosAngle = QVector3D::dotProduct(d1Vector, d2Vector) /
                         ((sqrt(pow(d1Vector.x(), 2) + pow(d1Vector.z(), 2))) *
                          (sqrt(pow(d2Vector.x(), 2) + pow(d2Vector.z(), 2))));

        float sinHalfAngle = sqrt((1 - cosAngle) / 2);
        float d3Radius = radius / sinHalfAngle;
        if (d3Radius > 1.2)
            d3Radius = 1.2;

        float d1Length = (sqrt(pow(d1Vector.x(), 2) + pow(d1Vector.z(), 2)));
        float d2Length = (sqrt(pow(d2Vector.x(), 2) + pow(d2Vector.z(), 2)));

        d1Vector /= d1Length;
        d2Vector /= d2Length;

        QVector3D d3Vector((d1Vector.x() + d2Vector.x())/2, d1Vector.y(), (d1Vector.z() + d2Vector.z()) /2 );
        float d3Length = (sqrt(pow(d3Vector.x(), 2) + pow(d3Vector.z(), 2)));
        d3Vector /= d3Length;

        float kValueDirection = d1Vector.x() * d2Vector.z() - d2Vector.x() * d1Vector.z();
        QVector3D k1, k2;
        if (kValueDirection > 0) {
            qDebug() << "kValue is greater than 0!";
            k1 = QVector3D(p2.x() - d3Radius * d3Vector.x(), p2.y(), p2.z() - d3Radius * d3Vector.z());
            k2 = QVector3D(p2.x() + d3Radius * d3Vector.x(), p2.y(), p2.z() + d3Radius * d3Vector.z());
        } else if (kValueDirection < 0) {
            qDebug() << "kValue is less than 0!";
            k1 = QVector3D(p2.x() + d3Radius * d3Vector.x(), p2.y(), p2.z() + d3Radius * d3Vector.z());
            k2 = QVector3D(p2.x() - d3Radius * d3Vector.x(), p2.y(), p2.z() - d3Radius * d3Vector.z());
        } else {
            qDebug() << "kValue equals to 0!";
            k1 = QVector3D(p2.x() + p1p2VertVector.x(), p2.y(), p2.z() + p1p2VertVector.z());
            k2 = QVector3D(p2.x() - p1p2VertVector.x(), p2.y(), p2.z() - p1p2VertVector.z());
        }

        QVector3D f1(p1.x() + p1p2VertVector.x(), p1.y(), p1.z() + p1p2VertVector.z());
        QVector3D f2(p1.x() - p1p2VertVector.x(), p1.y(), p1.z() - p1p2VertVector.z());

        QVector3D p1Vert1(p1.x(), p1.y() + shortRadius, p1.z());
        QVector3D p1Vert2(p1.x(), p1.y() - shortRadius, p1.z());

        QVector3D p2Vert1(p2.x(), p2.y() + shortRadius, p2.z());
        QVector3D p2Vert2(p2.x(), p2.y() - shortRadius, p2.z());

        if (!m_prevFacet.size()) {
            qDebug() << "###>> No previous facet!";

            gPushTriangleToList(f1, p1Vert1, f2);
            gPushTriangleToList(f1, f2     , p1Vert2);
            gPushTriangleToList(f1, k1     , p2Vert1);
            gPushTriangleToList(f1, p2Vert1, p1Vert1);
            gPushTriangleToList(f2, k2     , p1Vert1);
            gPushTriangleToList(k2, p2Vert1, p1Vert1);
            gPushTriangleToList(f1, p2Vert2, k1);
            gPushTriangleToList(f1, p1Vert2, p2Vert2);
            gPushTriangleToList(f2, k2     , p2Vert2);
            gPushTriangleToList(f2, p2Vert2, p1Vert2);

        } else {
            qDebug() << "###>> Got previous facet!";
            gPushTriangleToList(m_prevFacet.at(0), k1               , p2Vert1);
            gPushTriangleToList(m_prevFacet.at(0), p2Vert1          , m_prevFacet.at(2));
            gPushTriangleToList(m_prevFacet.at(1), k2               , m_prevFacet.at(2));
            gPushTriangleToList(k2               , p2Vert1          , m_prevFacet.at(2));
            gPushTriangleToList(m_prevFacet.at(0), p2Vert2          , k1);
            gPushTriangleToList(m_prevFacet.at(0), m_prevFacet.at(3), p2Vert2);
            gPushTriangleToList(m_prevFacet.at(1), k2               , p2Vert2);
            gPushTriangleToList(m_prevFacet.at(1), p2Vert2          , m_prevFacet.at(3));
        }
        m_prevFacet.clear();
        m_prevFacet.push_back(k1);
        m_prevFacet.push_back(k2);
        m_prevFacet.push_back(p2Vert1);
        m_prevFacet.push_back(p2Vert2);
        qDebug() << "--------↑↑↑ calc half-angle vector ↑↑↑-------";
    }
}


void GCode::recomputeAll()
{
    qDebug() << Q_FUNC_INFO;
    float oldx = 0, oldy = 0, oldz = 0;
    float minusX = maxX * 0.5;
    float minusY = maxY * 0.5;
    qDebug() << "codeLines size => " << codeLines.size();
    qDebug() << "minusX => " << minusX << " / minusY => " << minusY;
    for(unsigned int i = 0; i < codeLines.size(); i++) {

        if(codeLines[i].command == "G1") { 
            qDebug() << QString::fromUtf8(codeLines[i].clearedLine.c_str());
            if(codeLines[i].hasE && codeLines[i].hasXYZ) {
                QVector3D a(oldx, oldz, oldy);
                QVector3D b((codeLines[i].x-minusX), codeLines[i].z, codeLines[i].y-minusY);

                int nextIndex = i+1;
                if (nextIndex < codeLines.size()) {
                    GCodeLine nextLine = codeLines[nextIndex];
                    if (nextLine.hasXYZ) {
                        QVector3D c(nextLine.x-minusX, nextLine.z, nextLine.y-minusY);
                        if (!nextLine.hasE) { 
                            generateTube(a, b, c, true);
                        } else {
                            generateTube(a, b, c);
                        }
                    }
                } else {
                    QVector3D c(0,0,0);
                    generateTube(a, b, c, true);
                }
                oldx = codeLines[i].x - minusX;
                oldy = codeLines[i].y - minusY;
                oldz = codeLines[i].z;

            } else if(codeLines[i].hasXYZ) {
                m_prevFacet.clear();

                oldx = codeLines[i].x - minusX;
                oldy = codeLines[i].y - minusY;
                oldz = codeLines[i].z;
            }
        }
    }
    qDebug() << "~~~~~~~~~~~~~~~~~~~~~~\n";
    m_tubeIndices.resize(m_tubeVertices.size());
    m_tubeNormals.resize(m_tubeVertices.size());
    for (int i=0; i<m_tubeVertices.size(); i++) {
        m_tubeIndices[i] = i;
    }

    qDebug() << "index size => " << m_tubeIndices.size();
    for (int i = 0; i < m_tubeIndices.size(); i += 3) {
        const QVector3D a = m_tubeVertices.at(m_tubeIndices.at(i));
        const QVector3D b = m_tubeVertices.at(m_tubeIndices.at(i+1));
        const QVector3D c = m_tubeVertices.at(m_tubeIndices.at(i+2));

        const QVector3D normal = QVector3D::crossProduct(b - a, c - a).normalized();

        for (int j = 0; j < 3; ++j)
            m_tubeNormals[m_tubeIndices.at(i + j)] += normal;
    }
}

void GCode::setGCodeLayers(int layers) {
    qDebug() << Q_FUNC_INFO << layers;
    showLayers = layers;
}