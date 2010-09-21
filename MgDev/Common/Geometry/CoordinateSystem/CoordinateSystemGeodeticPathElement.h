//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#ifndef _MGCOORDINATESYSTEMGEODETICPATHELEMENT_H_
#define _MGCOORDINATESYSTEMGEODETICPATHELEMENT_H_

/// \defgroup MgCoordinateSystemGeodeticPathElement MgCoordinateSystemGeodeticPathElement
/// \ingroup Coordinate_System_classes
/// \{

////////////////////////////////////////////////////////////////
/// \brief
/// A geodetic transformation path element describes a single transformation step within a series
/// of concatenated steps that eventually form a geodetic (transformation) path.
///
class MG_GEOMETRY_API MgCoordinateSystemGeodeticPathElement : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgCoordinateSystemGeodeticPathElement)

PUBLISHED_API:

    virtual STRING GetTransformName() = 0;
    virtual void SetTransformName(CREFSTRING) = 0;

    virtual bool GetIsInversed() = 0;
    virtual void SetIsInversed(bool) = 0;

    virtual bool IsValid() = 0;
    virtual bool IsProtected() = 0;

protected:
    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the class Id
    ///
    /// \return
    /// The integer value
    ///
    INT32 GetClassId(){return m_cls_id;};

CLASS_ID:
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemGeodeticPathElement;
};

/// \}

#endif