/*
   CADET - Center for Advances in Digital Entertainment Technologies
   Copyright 2011 University of Applied Science Salzburg / MultiMediaTechnology
	   http://www.cadet.at

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#ifndef TARGET_MSKINECTSDK
#include "_2RealConfig.h"
#include "OpenNISpecific/OpenNIInfraredGenerator.h"
#include "_2RealImageSource.h"
#include "XnCppWrapper.h"


namespace _2Real
{

void XN_CALLBACK_TYPE infraredErrorStateChanged(xn::MapGenerator& rMapGenerator, void *pCookie)
{
	XnStatus status;
	xn::DepthGenerator *depthGenerator = static_cast<xn::DepthGenerator*>(pCookie);
	
	status = depthGenerator->GetErrorStateCap().GetErrorState();
	if (status != XN_STATUS_OK)
	{
		_2REAL_LOG(error) << "infrared generator is in error state; status: " << xnGetStatusString(status) << std::endl;
	}
}

void XN_CALLBACK_TYPE infraredOutputStateChanged(xn::MapGenerator& rMapGenerator, void *pCookie)
{
	_2REAL_LOG(info) << "infrared map resolution was changed\n";
}

OpenNIInfraredGenerator::OpenNIInfraredGenerator()
{
}

OpenNIInfraredGenerator::~OpenNIInfraredGenerator()
{
}

XnStatus OpenNIInfraredGenerator::startGenerating()
{
	m_InfraredGenerator.LockedNodeStartChanges(m_InfraredLock);
	XnStatus status = m_InfraredGenerator.StartGenerating();
	m_InfraredGenerator.LockedNodeEndChanges(m_InfraredLock);
	return (getErrorState() | status);
}

XnStatus OpenNIInfraredGenerator::getTimestamp( uint64_t& time ) const
{
	time = m_InfraredGenerator.GetTimestamp();
	return getErrorState();
}

XnStatus OpenNIInfraredGenerator::stopGenerating()
{
	m_InfraredGenerator.LockedNodeStartChanges(m_InfraredLock);
	XnStatus status = m_InfraredGenerator.StopGenerating();
	m_InfraredGenerator.LockedNodeEndChanges(m_InfraredLock);
	return (getErrorState() | status);
}

XnStatus OpenNIInfraredGenerator::lockGenerator()
{
	XnStatus status = m_InfraredGenerator.LockForChanges(&m_InfraredLock);
	return (getErrorState() | status);
}

XnStatus OpenNIInfraredGenerator::unlockGenerator()
{
	m_InfraredGenerator.UnlockForChanges(m_InfraredLock);	
	return getErrorState();
}

XnStatus OpenNIInfraredGenerator::getErrorState() const
{
	return m_InfraredGenerator.GetErrorStateCap().GetErrorState();
}

bool OpenNIInfraredGenerator::isGenerating() const
{
	if( m_InfraredGenerator.IsGenerating() == TRUE )
		return true;
	return false;
}

bool OpenNIInfraredGenerator::isMirrored() const
{
	if( m_InfraredGenerator.GetMirrorCap().IsMirrored() == TRUE )
		return true;
	return false;
}

XnStatus OpenNIInfraredGenerator::setOutputMode(const XnMapOutputMode outputMode)
{
	m_InfraredGenerator.LockedNodeStartChanges(m_InfraredLock);
	XnStatus status = m_InfraredGenerator.SetMapOutputMode(outputMode);
	m_InfraredGenerator.LockedNodeEndChanges(m_InfraredLock);
	return (getErrorState() | status);
}

XnStatus OpenNIInfraredGenerator::registerCallbacks()
{	
	XnCallbackHandle errorStateHandle;
	XnStatus status = m_InfraredGenerator.RegisterToNewDataAvailable((xn::StateChangedHandler)infraredErrorStateChanged, (xn::IRGenerator *)&m_InfraredGenerator, errorStateHandle);

	XnCallbackHandle outputStateHandle;
	status = m_InfraredGenerator.RegisterToMapOutputModeChange((xn::StateChangedHandler)infraredOutputStateChanged, (xn::IRGenerator *)&m_InfraredGenerator, outputStateHandle);
	
	return (getErrorState() | status);
}

XnStatus OpenNIInfraredGenerator::getData( _2RealImageSource<uint16_t>& data ) const
{
	xn::IRMetaData metadata;
	m_InfraredGenerator.GetMetaData(metadata);
	data.setData(m_InfraredGenerator.GetIRMap());
	data.setFullResolution(metadata.FullXRes(), metadata.FullYRes());
	data.setCroppedResolution(metadata.XRes(), metadata.YRes());
	data.setCroppingOffest(metadata.XOffset(), metadata.YOffset());
	data.setTimestamp(metadata.Timestamp());
	data.setFrameID(metadata.FrameID());
	data.setBytesPerPixel(metadata.BytesPerPixel());

	if (metadata.XOffset() != 0)
	{
		data.setCropping(true);
	}
	else
	{
		data.setCropping(false);
	}

	if (m_InfraredGenerator.GetMirrorCap().IsMirrored())
	{
		data.setMirroring(true);
	}
	else
	{
		data.setMirroring(false);
	}

	return getErrorState();
}

XnStatus OpenNIInfraredGenerator::setMirroring(const bool mirror)
{
	m_InfraredGenerator.LockedNodeStartChanges(m_InfraredLock);
	XnStatus status = m_InfraredGenerator.GetMirrorCap().SetMirror(mirror);
	m_InfraredGenerator.LockedNodeEndChanges(m_InfraredLock);
	return status;
}

XnStatus OpenNIInfraredGenerator::getFramesPerSecond( int& fps ) const
{
	XnMapOutputMode outputMode;
	m_InfraredGenerator.GetMapOutputMode(outputMode);
	fps = outputMode.nFPS;
	return getErrorState();
}

XnStatus OpenNIInfraredGenerator::getMapResolution( uint32_t& x, uint32_t& y ) const
{
	XnMapOutputMode outputMode;
	m_InfraredGenerator.GetMapOutputMode(outputMode);
	x = outputMode.nXRes;
	y = outputMode.nYRes;
	return getErrorState();
}

}

#endif
