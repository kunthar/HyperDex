// Copyright (c) 2011, Cornell University
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright notice,
//       this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of HyperDex nor the names of its contributors may be
//       used to endorse or promote products derived from this software without
//       specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#define __STDC_LIMIT_MACROS

// HyperDisk
#include "hyperdisk/snapshot.h"
#include "log_entry.h"
#include "shard_snapshot.h"

hyperdisk :: snapshot :: snapshot(std::vector<e::intrusive_ptr<hyperdisk::shard_snapshot> >* ss)
    : m_snaps()
    , m_ref(0)
{
    m_snaps.swap(*ss);
}

hyperdisk :: snapshot :: ~snapshot() throw ()
{
}

bool
hyperdisk :: snapshot :: valid()
{
    while (!m_snaps.empty())
    {
        if (m_snaps.back()->valid())
        {
            return true;
        }
        else
        {
            m_snaps.pop_back();
        }
    }

    return false;
}

void
hyperdisk :: snapshot :: next()
{
    if (!m_snaps.empty())
    {
        m_snaps.back()->next();
    }
}

uint32_t
hyperdisk :: snapshot :: primary_hash()
{
    if (!m_snaps.empty())
    {
        return m_snaps.back()->primary_hash();
    }
    else
    {
        return uint64_t();
    }
}

uint32_t
hyperdisk :: snapshot :: secondary_hash()
{
    if (!m_snaps.empty())
    {
        return m_snaps.back()->secondary_hash();
    }
    else
    {
        return uint64_t();
    }
}

bool
hyperdisk :: snapshot :: has_value()
{
    if (!m_snaps.empty())
    {
        return true;
    }
    else
    {
        return false;
    }
}

uint64_t
hyperdisk :: snapshot :: version()
{
    if (!m_snaps.empty())
    {
        return m_snaps.back()->version();
    }
    else
    {
        return uint64_t();
    }
}

e::buffer
hyperdisk :: snapshot :: key()
{
    if (!m_snaps.empty())
    {
        return m_snaps.back()->key();
    }
    else
    {
        return e::buffer();
    }
}

std::vector<e::buffer>
hyperdisk :: snapshot :: value()
{
    if (!m_snaps.empty())
    {
        return m_snaps.back()->value();
    }
    else
    {
        return std::vector<e::buffer>();
    }
}

hyperdisk :: rolling_snapshot :: rolling_snapshot(const e::locking_iterable_fifo<log_entry>::iterator& iter,
                                                  e::intrusive_ptr<snapshot> snap)
    : m_iter(iter)
    , m_snap(snap)
    , m_ref(0)
{
    valid();
}

hyperdisk :: rolling_snapshot :: ~rolling_snapshot() throw ()
{
}

bool
hyperdisk :: rolling_snapshot :: valid()
{
    return m_snap->valid() || m_iter.valid();
}

void
hyperdisk :: rolling_snapshot :: next()
{
    if (m_snap->valid())
    {
        m_snap->next();
    }
    else if (m_iter.valid())
    {
        m_iter.next();
    }
}

bool
hyperdisk :: rolling_snapshot :: has_value()
{
    if (m_snap->valid())
    {
        return m_snap->has_value();
    }
    else if (m_iter.valid())
    {
        return m_iter->coord.secondary_mask == UINT32_MAX;
    }
    else
    {
        return false;
    }
}

uint64_t
hyperdisk :: rolling_snapshot :: version()
{
    if (m_snap->valid())
    {
        return m_snap->version();
    }
    else if (m_iter.valid())
    {
        return m_iter->version;
    }
    else
    {
        return uint64_t();
    }
}

e::buffer
hyperdisk :: rolling_snapshot :: key()
{
    if (m_snap->valid())
    {
        return m_snap->key();
    }
    else if (m_iter.valid())
    {
        return m_iter->key;
    }
    else
    {
        return e::buffer();
    }
}

std::vector<e::buffer>
hyperdisk :: rolling_snapshot :: value()
{
    if (m_snap->valid())
    {
        return m_snap->value();
    }
    else if (m_iter.valid())
    {
        return m_iter->value;
    }
    else
    {
        return std::vector<e::buffer>();
    }
}