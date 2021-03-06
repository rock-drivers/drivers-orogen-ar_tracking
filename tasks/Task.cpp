/* Generated from orogen/lib/orogen/templates/tasks/Task.cpp */

#include "Task.hpp"
#include <ar_tracking/ArtDriver.hpp>

using namespace ar_tracking;

/* Sorry for the notation:
    - namespace ar_tracking for the orogen component
    - namespace artracking for the library (ARTDriver class)

*/

Task::Task(std::string const& name)
    : TaskBase(name),mpARTDriver(new artracking::ARTDriver())
{
}

Task::Task(std::string const& name, RTT::ExecutionEngine* engine)
    : TaskBase(name, engine),mpARTDriver(new artracking::ARTDriver())
{
}

Task::~Task()
{
}

::base::samples::RigidBodyState Task::getZeroOrigin()
{
    base::samples::RigidBodyState lrbs;
    lrbs.initUnknown();
    return lrbs;
}


/// The following lines are template definitions for the various state machine
// hooks defined by Orocos::RTT. See Task.hpp for more detailed
// documentation about them.

bool Task::configureHook()
{
    // No configuration needed apparently
    if (! TaskBase::configureHook())
        return false;

    if (!_origin.value().hasValidPosition() || !_origin.value().hasValidOrientation() ) 
        _origin.set(getZeroOrigin());

    if (!_body_reference.value().hasValidPosition() || !_body_reference.value().hasValidOrientation() ) 
        _body_reference.set(getZeroOrigin());

    return true;
}
bool Task::startHook()
{
    mpARTDriver->connect(_host.value(),_server_port.value(),_data_port.value());

    return true;
}
void Task::updateHook()
{
    mpARTDriver->getFrame();

    Eigen::Affine3d C_world2origin( Eigen::Affine3d(_origin.value()).inverse() );
    Eigen::Affine3d C_segment2body( Eigen::Affine3d(_body_reference.value()) );

    base::samples::RigidBodyState rbs;
    rbs.time = mpARTDriver->getTimestamp();

    rbs.sourceFrame = _source_frame.get();
    rbs.targetFrame = _target_frame.get();

    Eigen::Affine3d segment_transform = mpARTDriver->getSegmentTransform(_numBody.value());


    rbs.setTransform( C_world2origin * segment_transform * C_segment2body );

    _pose_samples.write( rbs );

}
void Task::errorHook()
{
    TaskBase::errorHook();
}
void Task::stopHook()
{
    mpARTDriver->disconnect();
}
void Task::cleanupHook()
{
    TaskBase::cleanupHook();
}
