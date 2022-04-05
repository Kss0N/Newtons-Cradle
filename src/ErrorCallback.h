/**
 * @file
 * @author  J. Keson <jakob.kristersson@bredband.net>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * https://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This is the Error Callback Interface for this PhysX API solution
 */
#pragma once

#include <physX/foundation/PxErrorCallback.h>
#include <physX/PxPhysicsAPI.h>

class ErrorCallback : public physx::PxErrorCallback
{
	void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line);
};
