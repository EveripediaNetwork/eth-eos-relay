/*
    This file is part of cpp-ethereum.

    cpp-ethereum is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    cpp-ethereum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with cpp-ethereum.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "WarpHostCapability.h"
#include "BlockChain.h"
#include "SnapshotStorage.h"

namespace dev
{
namespace eth
{
WarpHostCapability::WarpHostCapability(BlockChain const& _blockChain, u256 const& _networkId,
    std::shared_ptr<SnapshotStorageFace> _snapshotStorage)
  : m_blockChain(_blockChain), m_networkId(_networkId), m_snapshot(_snapshotStorage)
{
}

std::shared_ptr<p2p::Capability> WarpHostCapability::newPeerCapability(
    std::shared_ptr<p2p::SessionFace> const& _s, unsigned _idOffset, p2p::CapDesc const& _cap)
{
    auto ret = HostCapability<WarpPeerCapability>::newPeerCapability(_s, _idOffset, _cap);

    auto cap = p2p::capabilityFromSession<WarpPeerCapability>(*_s, _cap.second);
    assert(cap);
    cap->init(c_WarpProtocolVersion, m_networkId, m_blockChain.details().totalDifficulty,
        m_blockChain.currentHash(), m_blockChain.genesisHash(), m_snapshot);

    return ret;
}

}  // namespace eth
}  // namespace dev
